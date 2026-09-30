#include "routing/joint_segment.hpp"

#include "base/assert.hpp"



#include <sstream>

namespace routing
{
JointSegment::JointSegment(Segment const & from, Segment const & to)
{
  static_assert(sizeof(JointSegment) == 16, "");

  CHECK(!from.IsFakeCreated() && !to.IsFakeCreated(),
        ("Segments of joints can not be fake. Only through MakeFake() function."));

  CHECK_EQUAL(from.GetMwmId(), to.GetMwmId(), ());
  m_numMwmId = from.GetMwmId();

  CHECK_EQUAL(from.IsForward(), to.IsForward(), ());
  m_forward = from.IsForward();

  CHECK_EQUAL(from.GetFeatureId(), to.GetFeatureId(), ());
  m_featureId = from.GetFeatureId();

  m_startSegmentId = from.GetSegmentIdx();
  m_endSegmentId = to.GetSegmentIdx();
}

void JointSegment::AssignID(Segment const & seg)
{
  m_numMwmId = seg.GetMwmId();
  m_featureId = seg.GetFeatureId();
}

void JointSegment::AssignID(JointSegment const & seg)
{
  m_numMwmId = seg.m_numMwmId;
  m_featureId = seg.m_featureId;
}

JointSegment JointSegment::MakeFake(uint32_t fakeId, uint32_t featureId /* = kInvalidFeatureId*/)
{
  JointSegment res;
  res.m_featureId = featureId;
  res.m_startSegmentId = fakeId;
  res.m_endSegmentId = fakeId;
  res.m_numMwmId = kFakeNumMwmId;
  res.m_forward = false;
  return res;
}

bool JointSegment::IsFake() const
{
  // This is enough, but let's check in Debug for confidence.
  bool const result = (m_numMwmId == kFakeNumMwmId && m_startSegmentId != kInvalidSegmentId);
  if (result)
  {
    // Try if m_featureId can be real in a fake JointSegment.
    // ASSERT_EQUAL(m_featureId, kInvalidFeatureId, ());

    ASSERT_EQUAL(m_startSegmentId, m_endSegmentId, ());
    ASSERT_EQUAL(m_forward, false, ());
  }

  return result;
}

Segment JointSegment::GetSegment(bool start) const
{
  return {m_numMwmId, m_featureId, start ? m_startSegmentId : m_endSegmentId, m_forward};
}

std::string DebugPrint(JointSegment const & jointSegment)
{
  std::ostringstream out;
  if (jointSegment.IsFake())
    out << "[FAKE]";

  out << std::boolalpha << "JointSegment(" << jointSegment.GetMwmId() << ", " << jointSegment.GetFeatureId() << ", "
      << "[" << jointSegment.GetStartSegmentId() << " => " << jointSegment.GetEndSegmentId() << "], "
      << jointSegment.IsForward() << ")";
  return out.str();
}
}  // namespace routing
