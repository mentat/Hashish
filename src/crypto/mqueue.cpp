// mqueue.cpp - written and placed in the public domain by Wei Dai

#include "pch.h"
#include "mqueue.h"

NAMESPACE_BEGIN(CryptoPP)

MessageQueue::MessageQueue(unsigned int nodeSize)
	: m_queue(nodeSize), m_lengths(1, 0U), m_messageCounts(1, 0U)
{
}

unsigned int MessageQueue::CopyRangeTo2(BufferedTransformation &target, unsigned long &begin, unsigned long end, const std::string &channel, bool blocking) const
{
	if (begin >= MaxRetrievable())
		return 0;

	return m_queue.CopyRangeTo2(target, begin, STDMIN(MaxRetrievable(), end), channel, blocking);
}

unsigned int MessageQueue::TransferTo2(BufferedTransformation &target, unsigned long &transferBytes, const std::string &channel, bool blocking)
{
	transferBytes = STDMIN(MaxRetrievable(), transferBytes);
	unsigned int blockedBytes = m_queue.TransferTo2(target, transferBytes, channel, blocking);
	m_lengths.front() -= transferBytes;
	return blockedBytes;
}

bool MessageQueue::GetNextMessage()
{
	if (NumberOfMessages() > 0 && !AnyRetrievable())
	{
		m_lengths.pop_front();
		if (m_messageCounts[0] == 0 && m_messageCounts.size() > 1)
			m_messageCounts.pop_front();
		return true;
	}
	else
		return false;
}

unsigned int MessageQueue::CopyMessagesTo(BufferedTransformation &target, unsigned int count, const std::string &channel) const
{
	ByteQueue::Walker walker(m_queue);
	std::deque<unsigned long>::const_iterator it = m_lengths.begin();
	unsigned int i;
	for (i=0; i<count && it != --m_lengths.end(); ++i, ++it)
	{
		walker.TransferTo(target, *it, channel);
		if (GetAutoSignalPropagation())
			target.ChannelMessageEnd(channel, GetAutoSignalPropagation()-1);
	}
	return i;
}

void MessageQueue::swap(MessageQueue &rhs)
{
	m_queue.swap(rhs.m_queue);
	m_lengths.swap(rhs.m_lengths);
}

const byte * MessageQueue::Spy(unsigned int &contiguousSize) const
{
	const byte *result = m_queue.Spy(contiguousSize);
	contiguousSize = (unsigned int)STDMIN((unsigned long)contiguousSize, MaxRetrievable());
	return result;
}

// *************************************************************

unsigned int EqualityComparisonFilter::MapChannel(const std::string &channel) const
{
	if (channel == m_firstChannel)
		return 0;
	else if (channel == m_secondChannel)
		return 1;
	else
		return 2;
}

unsigned int EqualityComparisonFilter::ChannelPut2(const std::string &channel, const byte *inString, unsigned int length, int messageEnd, bool blocking)
{
	if (!blocking)
		throw BlockingInputOnly("EqualityComparisonFilter");

	unsigned int i = MapChannel(channel);

	if (i == 2)
		return Output(3, inString, length, messageEnd, blocking, channel);
	else if (m_mismatchDetected)
		return 0;
	else
	{
		MessageQueue &q1 = m_q[i], &q2 = m_q[1-i];

		if (q2.AnyMessages() && q2.MaxRetrievable() < length)
			goto mismatch;

		while (length > 0 && q2.AnyRetrievable())
		{
			unsigned int len = length;
			const byte *data = q2.Spy(len);
			len = STDMIN(len, length);
			if (memcmp(inString, data, len) != 0)
				goto mismatch;
			inString += len;
			length -= len;
			q2.Skip(len);
		}

		q1.Put(inString, length);

		if (messageEnd)
		{
			if (q2.AnyRetrievable())
				goto mismatch;
			else if (q2.AnyMessages())
				q2.GetNextMessage();
			else if (q2.NumberOfMessageSeries() > 0)
				goto mismatch;
			else
				q1.MessageEnd();
		}

		return 0;

mismatch:
		return HandleMismatchDetected(blocking);
	}
}

void EqualityComparisonFilter::ChannelInitialize(const std::string &channel, const NameValuePairs &parameters, int propagation)
{
	unsigned int i = MapChannel(channel);

	if (i == 2)
		PropagateInitialize(parameters, propagation, channel);
	else
	{
		m_q[i].Initialize();
		m_mismatchDetected = false;
	}
}

bool EqualityComparisonFilter::ChannelMessageSeriesEnd(const std::string &channel, int propagation, bool blocking)
{
	unsigned int i = MapChannel(channel);

	if (i == 2)
	{
		OutputMessageSeriesEnd(4, propagation, blocking, channel);
		return false;
	}
	else if (m_mismatchDetected)
		return false;
	else
	{
		MessageQueue &q1 = m_q[i], &q2 = m_q[1-i];

		if (q2.AnyRetrievable() || q2.AnyMessages())
			goto mismatch;
		else if (q2.NumberOfMessageSeries() > 0)
			return Output(2, (const byte *)"\1", 1, 0, blocking) != 0;
		else
			q1.MessageSeriesEnd();

		return false;

mismatch:
		return HandleMismatchDetected(blocking);
	}
}

bool EqualityComparisonFilter::HandleMismatchDetected(bool blocking)
{
	m_mismatchDetected = true;
	if (m_throwIfNotEqual)
		throw MismatchDetected();
	return Output(1, (const byte *)"\0", 1, 0, blocking) != 0;
}

NAMESPACE_END
