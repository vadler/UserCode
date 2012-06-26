import os
import FWCore.ParameterSet.Config as cms

files_Summer11 = [ '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_1_1_RA5.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_2_0_NUC.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_3_0_2FJ.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_4_0_0Dh.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_5_0_VKa.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_6_0_iFd.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_7_0_qsl.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_8_0_Auq.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_9_0_uYe.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_10_0_UU7.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_11_0_LU9.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_12_0_lLr.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_13_0_o6h.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_14_0_PTl.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_15_0_bt0.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_16_0_Uns.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_17_0_1Pr.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_18_0_lZ5.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_19_0_Lwt.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_20_0_cYJ.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_21_0_ej5.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_22_0_tdB.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_23_0_MJM.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_24_0_6NT.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_25_1_jPH.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_26_2_SHW.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_27_2_WgQ.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_28_2_fCQ.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_29_1_fpv.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_30_1_nD8.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_31_1_K32.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_32_1_L2p.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_33_1_WBP.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_34_1_5jV.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_35_1_Rys.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_36_1_fp6.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_37_0_ffT.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_38_0_ncm.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_39_0_sZg.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_40_0_FmI.root'
                 , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Summer11/skimHitFitResolutionFunctions_41_0_OMv.root'
                 ]

files_Fall11_R3 = [ '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_10_1_Lvc.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_11_1_XLH.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_12_1_KOl.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_13_0_WGd.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_14_0_Gl3.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_15_0_HUR.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_16_0_iP8.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_17_0_xVB.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_18_0_SI5.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_19_0_RKj.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_1_1_uGU.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_20_0_IUe.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_21_0_fNP.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_22_0_V1s.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_23_0_EpO.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_24_0_UIL.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_25_0_CUd.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_26_0_X8l.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_27_0_xbk.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_28_0_bed.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_29_0_lVF.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_2_1_M8T.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_30_0_jVF.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_31_0_HMv.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_32_0_n9K.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_33_0_2mq.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_34_0_MLl.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_35_0_s2Q.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_36_0_3qz.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_37_0_hNX.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_38_0_ez1.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_39_0_kCk.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_3_1_QNp.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_40_0_TeP.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_41_0_k8Q.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_42_0_Ldu.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_43_0_joR.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_4_1_OL7.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_5_1_hPR.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_6_1_e54.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_7_1_oLz.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_8_1_0MY.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_4/data/Fall11_R3/skimHitFitResolutionFunctions_9_1_Rfs.root'
                  ]

filesOld_Fall11_R3 = [ '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_10_0_zUr.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_11_0_AfC.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_12_0_kHg.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_13_0_fpU.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_14_0_Bqt.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_15_0_LRz.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_16_0_fvV.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_17_0_DAp.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_18_0_Bff.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_19_0_kiD.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_1_1_wTC.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_20_0_deo.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_21_0_EUy.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_22_0_pWU.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_23_0_iZq.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_24_0_CmW.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_25_0_VPD.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_26_0_lhk.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_27_0_DoW.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_28_0_KS8.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_29_0_sbV.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_2_0_rXT.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_30_0_6m6.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_31_0_IC5.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_32_0_9iL.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_33_0_egk.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_34_0_Nnk.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_35_0_B7W.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_36_0_Erd.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_37_0_3qK.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_38_0_lhR.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_39_0_IVX.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_3_0_MgK.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_40_0_o9T.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_41_0_PVV.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_42_0_Yeo.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_43_0_Gyc.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_4_0_7OS.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_5_0_NXg.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_6_0_YrP.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_7_0_gsK.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_8_0_FHW.root'
                  , '/store/user/vadler/cms/Top/CMSSW_4_4_2/data/Fall11_R3/skimHitFitResolutionFunctions_9_0_kjU.root'
                  ]
