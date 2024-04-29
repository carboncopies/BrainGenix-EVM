#pragma once

namespace BG {

enum ReportType: int {
    JSONReport,
    MDReport,
    NUMReportType
};

struct ValidationConfig {
    unsigned long Timeout_ms = 100000;
    unsigned int TryAngles = 12;
    ReportType ReportType_ = JSONReport;
};

} // BG
