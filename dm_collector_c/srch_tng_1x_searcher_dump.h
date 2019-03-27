/*
 * Srch_TNG_1x_Searcher_Dump
 */

#include "consts.h"
#include "log_packet.h"
#include "log_packet_helper.h"

const Fmt SrchTng1xsd_Fmt[] = {
    {UINT, "Version", 1},
    {UINT, "Number of Subpackets", 1},
    {SKIP, NULL, 2},
};

const Fmt SrchTng1xsd_Subpkt_header_v1[] = {
    {UINT, "Subpacket ID", 1},
    {UINT, "Subpacket Version", 1},
    {UINT, "Subpacket Size", 2},
};

const Fmt SrchTng1xsd_Subpkt_payload_2v2[] = {
    {UINT, "Tx Power", 2}, // x / 10.0
    {UINT, "Tx Gain Adj", 1},
    {UINT, "Tx Power Limit", 1}, // (x - 157) / 3
    {BYTE_STREAM, "Dev Mode Bit Mask", 1},
    {UINT, "Num Chains", 1},
    {UINT, "Diversity Enabled", 1},
    {SKIP, NULL, 1},
};

const Fmt SrchTng1xsd_RxChain_v2[] = {
    {UINT, "Client", 1},
    {UINT, "Band Class", 1},
    {UINT, "Channel", 2},
    {UINT, "Rx AGC", 1}, //  (x - 446) / 3
    {BYTE_STREAM, "Dev Cap Mask", 1},
    {UINT, "RF Dev Num", 1},
    {SKIP, NULL, 1},
};

const Fmt SrchTng1xsd_Subpkt_payload_7v1[] = {
    {UINT, "Technology", 1},       {UINT, "Queue", 1},
    {UINT, "Priority", 1},         {SKIP, NULL, 1},
    {UINT, "Frequency Offset", 2}, // x * 4.6875
    {UINT, "Num Tasks", 1},        {UINT, "Num Peaks", 1},
};

const Fmt SrchTng1xsd_Task_v1[] = {
    {UINT, "QOF", 2},                  // 2 bits
    {PLACEHOLDER, "HDR Burst Len", 0}, // 2 bits
    {PLACEHOLDER, "Div Antenna", 0},   // 2 bits
    {PLACEHOLDER, "Pilot PN", 0},      // 10 bits
    {UINT, "Coherent Length", 2},      // 13 bits
    {PLACEHOLDER, "Coh Trunc", 0},     // 3 bits
    {UINT, "Non Coh Len", 1},
    {UINT, "Pilot Phase", 1},
    {UINT, "Walsh Index", 2},
    {UINT, "Window Start", 2},
    {UINT, "Window Size", 2},
    {UINT, "Pilot Set", 4},
};

const Fmt SrchTng1xsd_Peak_v1[] = {
    {UINT, "Peak Position", 4},
    {UINT, "Peak Energy", 2},
    {SKIP, NULL, 2},
};

static int _decode_srch_tng_1xsd_payload(const char *b, int offset,
                                         size_t length, json &result) {
    int start = offset;
    int pkt_ver = _search_result_int(result, "Version");

    double pyfloat;
    int temp;

    switch (pkt_ver) {
    case 1: {
        int num_subpkts = _search_result_int(result, "Number of Subpackets");
        json result_allpkts;
        for (int i = 0; i < num_subpkts; i++) {
            json result_subpkt;
            int start_subpkt = offset;
            // Decode subpacket header
            offset +=
                _decode_by_fmt(SrchTng1xsd_Subpkt_header_v1,
                               ARRAY_SIZE(SrchTng1xsd_Subpkt_header_v1, Fmt), b,
                               offset, length, result_subpkt);
            int subpkt_id = _search_result_int(result_subpkt, "Subpacket ID");
            int subpkt_ver =
                _search_result_int(result_subpkt, "Subpacket Version");
            int subpkt_size =
                _search_result_int(result_subpkt, "Subpacket Size");

            if (subpkt_id == 2 && subpkt_ver == 2) {
                // this is RF subpkt v2
                offset += _decode_by_fmt(
                    SrchTng1xsd_Subpkt_payload_2v2,
                    ARRAY_SIZE(SrchTng1xsd_Subpkt_payload_2v2, Fmt), b, offset,
                    length, result_subpkt);

                temp = _search_result_int(result_subpkt, "Tx Power");
                float fTxPower = temp / 10.0;
                pyfloat = fTxPower;
                _replace_result(result_subpkt, "Tx Power", pyfloat);

                temp = _search_result_int(result_subpkt, "Tx Power Limit");
                float fTxPowerLimit = (temp - 157) / 3.0;
                pyfloat = fTxPowerLimit;
                _replace_result(result_subpkt, "Tx Power Limit", pyfloat);

                (void)_map_result_field_to_name(
                    result_subpkt, "Diversity Enabled",
                    ValueNameEnableOrDisable,
                    ARRAY_SIZE(ValueNameEnableOrDisable, ValueName),
                    "(MI)Unknown");

                int num_chains =
                    _search_result_int(result_subpkt, "Num Chains");
                json result_chain;
                for (int j = 0; j < num_chains; j++) {
                    json result_chain_item;

                    offset +=
                        _decode_by_fmt(SrchTng1xsd_RxChain_v2,
                                       ARRAY_SIZE(SrchTng1xsd_RxChain_v2, Fmt),
                                       b, offset, length, result_chain_item);

                    temp = _search_result_int(result_chain_item, "Rx AGC");
                    float fRxAgc = (temp - 446) / 3.0;
                    pyfloat = fRxAgc;
                    _replace_result(result_chain_item, "Rx AGC", pyfloat);

                    (void)_map_result_field_to_name(
                        result_chain_item, "Band Class", ValueNameBandClassCDMA,
                        ARRAY_SIZE(ValueNameBandClassCDMA, ValueName),
                        "(MI)Unknown");

                    result_chain.push_back(result_chain_item);
                }
                result_subpkt["Clients"] = result_chain;

            } else if (subpkt_id == 7 && subpkt_ver == 1) {
                // this is Searcher 4 subpkt v1
                offset += _decode_by_fmt(
                    SrchTng1xsd_Subpkt_payload_7v1,
                    ARRAY_SIZE(SrchTng1xsd_Subpkt_payload_7v1, Fmt), b, offset,
                    length, result_subpkt);

                (void)_map_result_field_to_name(
                    result_subpkt, "Technology", ValueNameTechnology,
                    ARRAY_SIZE(ValueNameTechnology, ValueName), "(MI)Unknown");
                (void)_map_result_field_to_name(
                    result_subpkt, "Queue", ValueNameQueue,
                    ARRAY_SIZE(ValueNameQueue, ValueName), "(MI)Unknown");
                temp = _search_result_int(result_subpkt, "Frequency Offset");
                float fFO = temp * 4.6875;
                pyfloat = fFO;
                _replace_result(result_subpkt, "Frequency Offset", pyfloat);
                int num_tasks = _search_result_int(result_subpkt, "Num Tasks");
                int num_peaks = _search_result_int(result_subpkt, "Num Peaks");

                json result_task;
                for (int j = 0; j < num_tasks; j++) {
                    json result_task_item;
                    offset +=
                        _decode_by_fmt(SrchTng1xsd_Task_v1,
                                       ARRAY_SIZE(SrchTng1xsd_Task_v1, Fmt), b,
                                       offset, length, result_task_item);
                    temp = _search_result_int(result_task_item, "QOF");
                    int iQOF = temp & 3;           // 2 bits
                    int iHBL = (temp >> 2) & 3;    // 2 bits
                    int iDA = (temp >> 4) & 3;     // 2 bits
                    int iPPN = (temp >> 6) & 1023; // 10 bits
                    temp =
                        _search_result_int(result_task_item, "Coherent Length");
                    int iCL = temp & 8191;      // 13 bits
                    int iCT = (temp >> 13) & 7; // 3 bits
                    _replace_result_int(result_task_item, "QOF", iQOF);
                    _replace_result_int(result_task_item, "HDR Burst Len",
                                        iHBL);
                    _replace_result_int(result_task_item, "Div Antenna", iDA);
                    _replace_result_int(result_task_item, "Pilot PN", iPPN);
                    _replace_result_int(result_task_item, "Coherent Length",
                                        iCL);
                    _replace_result_int(result_task_item, "Coh Trunc", iCT);

                    unsigned int utemp =
                        _search_result_uint(result_task_item, "Pilot Set");

                    if (utemp & 1)
                        utemp = 1;
                    else if (utemp & 2)
                        utemp = 2;
                    else if (utemp & 4)
                        utemp = 4;
                    else if (utemp & 8)
                        utemp = 8;
                    else if (!(utemp & 15) | 0)
                        utemp = 0;
                    _replace_result_int(result_task_item, "Pilot Set", utemp);
                    (void)_map_result_field_to_name(
                        result_task_item, "Pilot Set", ValueNamePilotSet,
                        ARRAY_SIZE(ValueNamePilotSet, ValueName),
                        "(MI)Unknown");

                    json result_peak;
                    for (int k = 0; k < num_peaks; k++) {
                        json result_peak_item;
                        offset +=
                            _decode_by_fmt(SrchTng1xsd_Peak_v1,
                                           ARRAY_SIZE(SrchTng1xsd_Peak_v1, Fmt),
                                           b, offset, length, result_peak_item);
                        result_peak.push_back(result_peak_item);
                    }
                    result_task_item["Peaks"] = result_peak;

                    result_task.push_back(result_task_item);
                }
                result_subpkt["Tasks"] = result_task;

            } else {
                printf("(MI)Unknown LTE PHY Idle Neighbor Cell Meas subpkt"
                       " id and version: 0x%x - %d\n",
                       subpkt_id, subpkt_ver);
            }

            result_allpkts.push_back(result_subpkt);
            offset += subpkt_size - (offset - start_subpkt);
        }
        result["Subpackets"] = result_allpkts;
        return offset - start;
    }
    default:
        printf("(MI)Unknown Srch TNG 1x Searcher Dump version: 0x%x\n",
               pkt_ver);
        return 0;
    }
}
