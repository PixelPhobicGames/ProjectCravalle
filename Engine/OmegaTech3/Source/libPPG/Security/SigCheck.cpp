#include "SigCheck.hpp"

template <typename Type>
bool VerifyData(Type Data, Type Signature) {
    int SizeOfData = Data.size();

    int Divisor = 4;

    bool Check = true;

    if (int((Signature.size())) < SizeOfData / Divisor)
        return false;

    for (int i = 0; i <= SizeOfData; i += Divisor) {
        if (Data[i] != Signature[i / Divisor]) {
            Check = false;
        }
    }

    return Check;
}

template bool VerifyData<std::string>(std::string Data, std::string Signature);

template <typename Type>
Type SignatureGen(Type Data) {
    int SizeOfData = Data.size();

    int Divisor = 4;

    Type Signature;

    for (int i = 0; i <= SizeOfData; i += Divisor) {
        Signature += Data[i];
    }

    return Signature;
}
template std::string SignatureGen<std::string>(std::string Data);

string Signatures[20];

void InitSig() {
    Signatures[1] =
        "Hhp00010P:e41188-:.:e13.11.6:o2.0:.130Ml2:88110Ml8:51::0Ml60:.120Ml6:41::.:e2588809Cd:0.38.4:o2.03204Cd:90:."
        "120Ml5:36::.:e151540.:e2434258Cd:30:.030Ml2:89::.:e2509::0Ml5:33::.:e6355957Cd:80:.030Ml0907359Cd:.2:."
        "030Ml4437::.:e10.6500Cd:.3:8.3:o22:3..5:o29:4.120Ml66.1309Cd:.-2.120Ml12.8::.:e331:2.0:o28::106Cd:.-7."
        "130Ml6809.3Cd:.-57.5:o21:63::.:e441:3.7:o22-7.130Ml912.110Ml270.110Ml5376::.:e55.3108Cd:.-5.120Ml2338::.:e68."
        "690.:e65.7:::o20410.0:o27:79::.:e77.13..:e78.14.1:o20:10500Cd:.-89::.:e763:.120Ml97.0702Cd:.0:.14:o21-70::.:"
        "e771:.17:o21:03::.:e771:.120Ml9:05120Ml88.58.9Cd:.-7713:o221:.120Ml591220.:e75.68.7Cd:..65.8:o24:11::.:e64.72."
        "6Cd:.0:.110Ml3624::.:e670:.12:o21:86101Cd:.0:.130Ml08.9::.:e401:.130Ml42.9707Cd:.-71::.:e321:.17:o26:69704Cd:."
        "-59::.:e230:.11:o28:67506Cd:.-50::.:e27.590.:e27.4::.:e19.74.1Cd:.-94::0Ml4320300Ml2337909Cd:.3:.120Ml9:48::.:"
        "e745030.:e5377902Cd:7.50.5:o2.-29::.:e2416508Cd:.164::.:e37021.4Cd:.169::.:e13.6107:Md:.282:::e235:.11Ml07."
        "2111Ml95.58..o51:14:00d:.329:00d:.1:.1::e3623711Ml62.33..o55:17:00d:.278:00d:.298:00d:.115:00d:.1201::e306:.1:"
        ":e34.6711Ml78917..o56:73:00d:.176:00d:.1201::e486:.1::e531:.1::e68.6411Ml6:95:00d:.8:.1::e610:.1::e72.3611Ml6:"
        "611..o52:35:00d:..7:00d:.4:911Ml7:48:00d:.153:00d:..2611Ml55.2711Ml33.3511Ml24.4811Ml53.5811Cd:.8:.11Cd:..14.."
        ":e42.18..:e27.7650Ml1390750Ml8661750d:.231:::e281:.91Ml75.6700d:.325:::e3513300d:.269:::e579:.91Ml84.32..o64:"
        "25:::e412:.91Ml25.58..o68:38200d:.17491:Md:.6-3018650:00vlo760.1257.010:.256715:i:.237010:0000ct738.0311.010::"
        ".837030:0000o831:471440010:e125.7223.0900r326:6723900.0Sp12331:.240:00";
}
