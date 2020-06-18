#pragma once

/**
 * \class ParticipantSingleton.hpp
 * \brief Domain Participant Singleton for 
 * Domain 0 - this interface can be used to retrieve 
 * a Participant to create reader / writer / ... for 
 * this Domain
 * A domain participant is required to create reader 
 * and writer / publisher and subscriber. This interface 
 * can be used to retrieve the "default" participant that 
 * is used to communicate with e.g. the vehicles. Unless 
 * communication in another domain or QoS settings for the 
 * participant (instead of QoS settings for reader / writer / ...) 
 * are required, use this participant.
 */

#include <dds/domain/DomainParticipant.hpp>

namespace cpm 
{
    class ParticipantSingleton
    {
        ParticipantSingleton() = delete;
        ParticipantSingleton(ParticipantSingleton const&) = delete;
        ParticipantSingleton(ParticipantSingleton&&) = delete; 
        ParticipantSingleton& operator=(ParticipantSingleton const&) = delete;
        ParticipantSingleton& operator=(ParticipantSingleton &&) = delete;

    public:
        /**
         * \brief Retrieve the participant singleton with this function
         * \return A participant
         */
        static dds::domain::DomainParticipant& Instance();

    };
}