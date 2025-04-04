// IEventsHandlerMock.hpp 
#pragma once

#include "Application/IEventsHandler.hpp" 
#include <gmock/gmock.h>
#include <optional>

namespace ue
{

    class IEventsHandlerMock : public IEventsHandler
    {
    public:
        // Mock methods from IBtsEventsHandler
        MOCK_METHOD(void, handleSib, (common::BtsId), (override));
        MOCK_METHOD(void, handleAttachAccept, (), (override));
        MOCK_METHOD(void, handleAttachReject, (), (override));
        MOCK_METHOD(void, handleDisconnected, (), (override));
        MOCK_METHOD(void, handleSmsReceived, (common::PhoneNumber from, std::string text), (override));
        MOCK_METHOD(void, handleSmsSentResult, (common::PhoneNumber to, bool success), (override));
        MOCK_METHOD(void, handleSmsComposeResult, (common::PhoneNumber recipient, const std::string& text), (override));

        // Mock methods from ITimerEventsHandler
        MOCK_METHOD(void, handleTimeout, (), (override));

        // Mock methods from IUserEventsHandler (empty base) - no methods here

        // Mock methods added in IEventsHandler itself
        MOCK_METHOD(void, handleUiAction, (std::optional<std::size_t> selectedIndex), (override));
        MOCK_METHOD(void, handleUiBack, (), (override));
    };

} // namespace ue