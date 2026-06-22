// gratten_limits.h
// Gratten GA1483 / Atten Technology command definitions.
// SCPI structure is near-identical to RIGOL DSG800 and Siglent SSG3000X.
// Reference: RIGOL DSG800 Programming Guide
#pragma once

#include <QString>
#include <QVector>

struct GrattenCommandInfo {
    QString key;
    QString setPattern;
    QString queryCmd;
    double minVal;
    double maxVal;
    QString unit;
    bool isBoolean;
};

inline const QVector<GrattenCommandInfo> GRATTEN_COMMANDS = {
    // ── Carrier frequency ─────────────────────────────────────────────────
    {"FREQ",       ":FREQuency:CW %1Hz",            ":FREQuency:CW?",          250e3,  4e9,     "Hz",   false},

    // ── Output power ──────────────────────────────────────────────────────
    {"AMPL",       ":POWer:LEVEL %1DBM",             ":POWer:LEVEL?",          -136.0, 13.0,    "dBm",  false},

    // ── RF output ─────────────────────────────────────────────────────────
    {"OUTP",       ":OUTPut:STATE %1",               ":OUTPut:STATE?",           0,     1,      "",     true},

    // ── AM modulation ─────────────────────────────────────────────────────
    {"AMFREQ",     ":AM:INTernal:FREQuency %1Hz",    ":AM:INTernal:FREQuency?",  20,    20000,  "Hz",   false},
    {"AMDEPTH",    ":AM:DEPTh %1%",                  ":AM:DEPTh?",               0,     100,    "%",    false},
    {"AMSTATE",    ":AM:STATE %1",                   ":AM:STATE?",               0,     1,      "",     true},

    // ── FM modulation (not yet wired to high-level API) ───────────────────
    {"FMSTATE",    ":FM:STATe %1",                   ":FM:STATe?",               0,     1,      "",     true},
    {"FMDEV",      ":FM:DEViation %1",               ":FM:DEViation?",           0,     1e6,    "Hz",   false},
    {"FMFREQ",     ":FM:INTernal:FREQuency %1",      ":FM:INTernal:FREQuency?",  20,    20000,  "Hz",   false},

    // ── PM modulation (not yet wired to high-level API) ───────────────────
    {"PMSTATE",    ":PM:STATe %1",                   ":PM:STATe?",               0,     1,      "",     true},
    {"PMDEV",      ":PM:DEViation %1",               ":PM:DEViation?",           0,     31.4,   "rad",  false},
    {"PMFREQ",     ":PM:INTernal:FREQuency %1",      ":PM:INTernal:FREQuency?",  20,    20000,  "Hz",   false},

    // ── Sweep (not yet wired to high-level API) ───────────────────────────
    {"SWPSTATE",   ":SWEep:STATe %1",                ":SWEep:STATe?",            0,     1,      "",     true},
    {"SWPSTART",   ":SWEep:FREQuency:STARt %1Hz",    ":SWEep:FREQuency:STARt?",  250e3, 4e9,    "Hz",   false},
    {"SWPSTOP",    ":SWEep:FREQuency:STOP %1Hz",     ":SWEep:FREQuency:STOP?",   250e3, 4e9,    "Hz",   false},
    {"SWPDWELL",   ":SWEep:DWELl %1",                ":SWEep:DWELl?",            1e-3,  100,    "s",    false},

    // ── Pulse modulation (not yet wired to high-level API) ────────────────
    {"PULMSTATE",  ":PULM:STATe %1",                 ":PULM:STATe?",             0,     1,      "",     true},
    {"PULMPERIOD", ":PULM:INTernal:PERiod %1",       ":PULM:INTernal:PERiod?",   1e-6,  1,      "s",    false},
    {"PULMWIDTH",  ":PULM:INTernal:WIDTh %1",        ":PULM:INTernal:WIDTh?",    500e-9, 1,     "s",    false},

    // ── LF output (not yet wired to high-level API) ───────────────────────
    {"LFSTATE",    ":LFOutput:STATe %1",             ":LFOutput:STATe?",          0,    1,      "",     true},
    {"LFFREQ",     ":LFOutput:FREQuency %1",         ":LFOutput:FREQuency?",      20,   200e3,  "Hz",   false},
    {"LFAMPL",     ":LFOutput:AMPLitude %1",         ":LFOutput:AMPLitude?",      0,    3.0,    "V",    false},

    // ── Display (utility) ─────────────────────────────────────────────────
    {"DISPUPD",    ":DISPlay:UPDate %1",             ":DISPlay:UPDate?",          0,     1,      "",     true},
};
