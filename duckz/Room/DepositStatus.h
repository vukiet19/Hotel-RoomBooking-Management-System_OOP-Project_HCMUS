#pragma once

enum class DepositStatus
{
    NONE,
    PENDING, // khách chx trả tiền reservation
    HELD,    // mình giữ tiền khách
    RETURNED // mình trả tiền lại tiền cọc cho khách ở quầy check-in
};