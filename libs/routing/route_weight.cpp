#include "routing/route_weight.hpp"

#include "routing/cross_mwm_connector.hpp"



namespace routing
{

int RouteWeight::s_PassThroughPenaltyS = 30 * 60;  // 30 minutes
int RouteWeight::s_AccessPenaltyS = 2 * 60 * 60;   // 2 hours

double RouteWeight::ToCrossMwmWeight() const
{
  // Do not accumulate pass-through or access-change edges into cross-mwm graph.
  /// @todo This is not very honest, but we have thousands of enter-exit edges
  /// in cross-mwm graph now and should filter them somehow.

  if (m_numPassThroughChanges > 0 || m_numAccessChanges > 0)
    return connector::kNoRoute;
  return GetWeight();
}



std::ostream & operator<<(std::ostream & os, RouteWeight const & routeWeight)
{
  os << "(" << static_cast<int32_t>(routeWeight.GetNumPassThroughChanges()) << ", "
     << static_cast<int32_t>(routeWeight.m_numAccessChanges) << ", "
     << static_cast<int32_t>(routeWeight.m_numAccessConditionalPenalties) << ", " << routeWeight.GetWeight() << ", "
     << routeWeight.GetTransitTime() << ")";
  return os;
}

RouteWeight operator*(double lhs, RouteWeight const & rhs)
{
  return RouteWeight(lhs * rhs.GetWeight(), rhs.GetNumPassThroughChanges(), rhs.m_numAccessChanges,
                     rhs.m_numAccessConditionalPenalties, lhs * rhs.GetTransitTime());
}
}  // namespace routing
