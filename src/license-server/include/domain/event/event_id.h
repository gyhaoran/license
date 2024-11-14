#ifndef DD50D815_D521_4569_80F3_9B699D61A88B
#define DD50D815_D521_4569_80F3_9B699D61A88B

namespace lic
{

using EventId = unsigned int;

constexpr EventId EV_BEGIN = 1000;
constexpr EventId EV_AUTHRIZATION_REQ = EV_BEGIN + 1;
constexpr EventId EV_INSTANCE_REL     = EV_BEGIN + 2;
constexpr EventId EV_INSTANCE_ECHO    = EV_BEGIN + 3;

constexpr EventId EV_INVALID_ID = 0xFFFFFFFF;


#define EVENT_ID_STR(id) #id

constexpr const char* event_id_to_string(EventId id) 
{
    switch (id) 
    {
        case EV_AUTHRIZATION_REQ: return EVENT_ID_STR(EV_AUTHRIZATION_REQ);
        case EV_INSTANCE_REL: return EVENT_ID_STR(EV_INSTANCE_REL);
        case EV_INSTANCE_ECHO: return EVENT_ID_STR(EV_INSTANCE_ECHO);
        case EV_INVALID_ID: return EVENT_ID_STR(EV_INVALID_ID);
        default: return "Unknown EventId";
    }
}

inline const char* ev_id_str(EventId id) 
{
    return event_id_to_string(id);
}

} // namespace lic

#endif /* DD50D815_D521_4569_80F3_9B699D61A88B */
