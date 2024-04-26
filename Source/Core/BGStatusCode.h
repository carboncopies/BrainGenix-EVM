#pragma once

enum BGStatusCode {
    BGStatusSuccess = 0,
    BGStatusGeneralFailure = 1,
    BGStatusInvalidParametersPassed = 2,
    BGStatusUpstreamGatewayUnavailable = 3,
    BGStatusUnauthorizedInvalidNoToken = 4,
    BGStatusSimulationBusy = 5,
    NUMBGStatusCode
};
