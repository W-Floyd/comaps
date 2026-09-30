#pragma once

#include "routing/route_weight.hpp"
#include "routing/segment.hpp"

#include "routing_common/num_mwm_id.hpp"

#include "indexer/feature_decl.hpp"

#include <limits>
#include <string>

namespace routing
{
class JointSegment
{
public:
  static auto constexpr kInvalidSegmentId = std::numeric_limits<uint32_t>::max();

  JointSegment() = default;
  JointSegment(Segment const & from, Segment const & to);

  uint32_t GetFeatureId() const { return m_featureId; }
  NumMwmId GetMwmId() const { return m_numMwmId; }
  uint32_t GetStartSegmentId() const { return m_startSegmentId; }
  uint32_t GetEndSegmentId() const { return m_endSegmentId; }
  uint32_t GetSegmentId(bool start) const { return start ? m_startSegmentId : m_endSegmentId; }
  bool IsForward() const { return m_forward; }

  void AssignID(Segment const & seg);
  void AssignID(JointSegment const & seg);

  static JointSegment MakeFake(uint32_t fakeId, uint32_t featureId = kInvalidFeatureId);
  bool IsFake() const;

  Segment GetSegment(bool start) const;

  bool operator<(JointSegment const & rhs) const
  {
    if (m_featureId != rhs.m_featureId)
      return m_featureId < rhs.m_featureId;
    if (m_forward != rhs.m_forward)
      return m_forward < rhs.m_forward;
    if (m_startSegmentId != rhs.m_startSegmentId)
      return m_startSegmentId < rhs.m_startSegmentId;
    if (m_endSegmentId != rhs.m_endSegmentId)
      return m_endSegmentId < rhs.m_endSegmentId;
    return m_numMwmId < rhs.m_numMwmId;
  }

  bool operator==(JointSegment const & rhs) const
  {
    return m_featureId == rhs.m_featureId && m_startSegmentId == rhs.m_startSegmentId &&
           m_endSegmentId == rhs.m_endSegmentId && m_numMwmId == rhs.m_numMwmId &&
           m_forward == rhs.m_forward;
  }

  bool operator!=(JointSegment const & rhs) const { return !(*this == rhs); }

private:
  uint32_t m_featureId = kInvalidFeatureId;

  uint32_t m_startSegmentId = kInvalidSegmentId;
  uint32_t m_endSegmentId = kInvalidSegmentId;

  NumMwmId m_numMwmId = kFakeNumMwmId;
  bool m_forward = false;
};

class JointEdge
{
public:
  JointEdge() = default;  // needed for buffer_vector only
  JointEdge(JointSegment const & target, RouteWeight const & weight) : m_target(target), m_weight(weight) {}

  JointSegment const & GetTarget() const { return m_target; }
  JointSegment & GetTarget() { return m_target; }
  RouteWeight & GetWeight() { return m_weight; }
  RouteWeight const & GetWeight() const { return m_weight; }

private:
  JointSegment m_target;
  RouteWeight m_weight;
};

std::string DebugPrint(JointSegment const & jointSegment);
}  // namespace routing

namespace std
{
template <>
struct hash<routing::JointSegment>
{
  size_t operator()(routing::JointSegment const & js) const noexcept
  {
    uint64_t const w1 = (static_cast<uint64_t>(js.GetFeatureId()) << 32) | js.GetStartSegmentId();
    uint64_t const w2 = (static_cast<uint64_t>(js.GetEndSegmentId()) << 32) |
                        (static_cast<uint64_t>(js.GetMwmId()) << 1) | (js.IsForward() ? 1ULL : 0ULL);
    uint64_t h = w1 ^ (w2 * 0x9e3779b97f4a7c15ULL);
    h ^= h >> 30;
    h *= 0xbf58476d1ce4e5b9ULL;
    h ^= h >> 27;
    h *= 0x94d049bb133111ebULL;
    h ^= h >> 31;
    return static_cast<size_t>(h);
  }
};
}  // namespace std
