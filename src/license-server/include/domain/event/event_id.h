#ifndef DD50D815_D521_4569_80F3_9B699D61A88B
#define DD50D815_D521_4569_80F3_9B699D61A88B

namespace lic
{

using EventId = unsigned int;

constexpr EventId EV_BEGIN = 1000;
constexpr EventId EV_AUTHRIZATION_REQ = EV_BEGIN + 1;
constexpr EventId EV_INSTANCE_REL     = EV_BEGIN + 2;

constexpr EventId EV_INVALID_ID = 0xFFFFFFFF;

} // namespace lic Authorization



#endif /* DD50D815_D521_4569_80F3_9B699D61A88B */
