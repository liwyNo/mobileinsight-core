/*
 * GSM_RR_Cell_Reselection_Meas
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt GsmRrCellResMeas_Fmt[] = {
    {UINT, "Serving BCCH ARFCN", 2},        // 12 bits
    {PLACEHOLDER, "Serving BCCH Band", 0},  // 4 bits
    {UINT, "Serving PBCCH ARFCN", 2},       // 12 bits
    {PLACEHOLDER, "Serving PBCCH Band", 0}, // 4 bits
    {UINT, "Serving Priority Class", 1},
    {UINT, "Serving RX Level Average", 1}, // x - 110.5
    {UINT, "Serving C1 Value", 4},
    {UINT, "Serving C2 Value", 4},
    {UINT, "Serving C31 Value", 4},
    {UINT, "Serving C32 Value", 4},
    {UINT, "Serving 5 Sec Timer State", 1},
    {UINT, "Serving Barred for Reselection", 1},
    {UINT, "Serving Cell Reselection in Last 15 Seconds", 1},
    {UINT, "Neighboring 6 Strongest Cells Count", 1},
};

const Fmt GsmRrCellResMeas_NeighborCell[] = {
    {UINT, "BCCH ARFCN", 2},        // 12 bits
    {PLACEHOLDER, "BCCH Band", 0},  // 4 bits
    {UINT, "PBCCH ARFCN", 2},       // 12 bits
    {PLACEHOLDER, "PBCCH Band", 0}, // 4 bits
    {UINT, "Priority Class", 1},
    {UINT, "RX Level Average", 1}, // x - 110.5
    {UINT, "C1 Value", 4},
    {UINT, "C2 Value", 4},
    {UINT, "C31 Value", 4},
    {UINT, "C32 Value", 4},
    {UINT, "5 Sec Timer State", 1},
    {UINT, "Barred for Reselection", 1},
    {UINT, "Belong to Serving Cell's RA", 1},
};

static int _decode_gsm_rcrm_payload(const char *b, int offset, size_t length,
                                    json &result) {
    int start = offset;

    double pyfloat;
    int temp;

    temp = _search_result_int(result, "Serving BCCH ARFCN");
    int iServingBCCH = temp & 4095; // 12 bits
    int iServingBCCHBand = (temp >> 12) & 15;
    _replace_result_int(result, "Serving BCCH ARFCN", iServingBCCH);
    _replace_result_int(result, "Serving BCCH Band", iServingBCCHBand);
    (void)_map_result_field_to_name(
        result, "Serving BCCH Band", ValueNameBandClassGSM,
        ARRAY_SIZE(ValueNameBandClassGSM, ValueName), "(MI)Unknown");

    temp = _search_result_int(result, "Serving PBCCH ARFCN");
    int iServingPBCCH = temp & 4095; // 12 bits
    int iServingPBCCHBand = (temp >> 12) & 15;
    _replace_result_int(result, "Serving PBCCH ARFCN", iServingPBCCH);
    _replace_result_int(result, "Serving PBCCH Band", iServingPBCCHBand);
    (void)_map_result_field_to_name(
        result, "Serving PBCCH Band", ValueNameBandClassGSM,
        ARRAY_SIZE(ValueNameBandClassGSM, ValueName), "(MI)Unknown");

    temp = _search_result_int(result, "Serving RX Level Average");
    float fRx = temp - 110.5;
    pyfloat = fRx;
    _replace_result(result, "Serving RX Level Average", pyfloat);

    (void)_map_result_field_to_name(
        result, "Serving 5 Sec Timer State", ValueNameTimerState,
        ARRAY_SIZE(ValueNameTimerState, ValueName), "(MI)Unknown");

    (void)_map_result_field_to_name(
        result, "Serving Barred for Reselection", ValueNameYesOrNo,
        ARRAY_SIZE(ValueNameYesOrNo, ValueName), "(MI)Unknown");

    (void)_map_result_field_to_name(
        result, "Serving Cell Reselection in Last 15 Seconds", ValueNameYesOrNo,
        ARRAY_SIZE(ValueNameYesOrNo, ValueName), "(MI)Unknown");

    int num_record =
        _search_result_int(result, "Neighboring 6 Strongest Cells Count");
    json result_record;
    for (int i = 0; i < num_record; i++) {
        json result_record_item;
        offset += _decode_by_fmt(GsmRrCellResMeas_NeighborCell,
                                 ARRAY_SIZE(GsmRrCellResMeas_NeighborCell, Fmt),
                                 b, offset, length, result_record_item);

        temp = _search_result_int(result_record_item, "BCCH ARFCN");
        int iBCCH = temp & 4095; // 12 bits
        int iBCCHBand = (temp >> 12) & 15;
        _replace_result_int(result_record_item, "BCCH ARFCN", iBCCH);
        _replace_result_int(result_record_item, "BCCH Band", iBCCHBand);
        (void)_map_result_field_to_name(
            result_record_item, "BCCH Band", ValueNameBandClassGSM,
            ARRAY_SIZE(ValueNameBandClassGSM, ValueName), "(MI)Unknown");

        temp = _search_result_int(result_record_item, "PBCCH ARFCN");
        int iPBCCH = temp & 4095; // 12 bits
        int iPBCCHBand = (temp >> 12) & 15;
        _replace_result_int(result_record_item, "PBCCH ARFCN", iPBCCH);
        _replace_result_int(result_record_item, "PBCCH Band", iPBCCHBand);
        (void)_map_result_field_to_name(
            result_record_item, "PBCCH Band", ValueNameBandClassGSM,
            ARRAY_SIZE(ValueNameBandClassGSM, ValueName), "(MI)Unknown");

        temp = _search_result_int(result_record_item, "RX Level Average");
        float fRx = temp - 110.5;
        pyfloat = fRx;
        _replace_result(result_record_item, "RX Level Average", pyfloat);

        (void)_map_result_field_to_name(
            result_record_item, "5 Sec Timer State", ValueNameTimerState,
            ARRAY_SIZE(ValueNameTimerState, ValueName), "(MI)Unknown");

        (void)_map_result_field_to_name(
            result_record_item, "Barred for Reselection", ValueNameYesOrNo,
            ARRAY_SIZE(ValueNameYesOrNo, ValueName), "(MI)Unknown");

        (void)_map_result_field_to_name(
            result_record_item, "Belongs to Serving Cell's RA",
            ValueNameYesOrNo, ARRAY_SIZE(ValueNameYesOrNo, ValueName),
            "(MI)Unknown");

        result_record.push_back(result_record_item);
    }
    result["Neighboring 6 Strongest Cells"] = result_record;
    return offset - start;
}
