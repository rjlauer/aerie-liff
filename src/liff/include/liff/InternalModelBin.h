#ifndef INTERNAL_MODEL_BIN_H
#define INTERNAL_MODEL_BIN_H

#include <liff/BinList.h>
#include <liff/InternalModel.h>
#include <liff/skymaps/SkyMap.h>

/*!
 * @class InternalModelBin
 * @author Robert Lauer
 * @date 9 July 2015
 * @ingroup 
 * @brief wrapper of InternalModel each CalcBin, with individual BGModel
 */


class InternalModelBin {

  public:

    ///Very basic constructor
    InternalModelBin();

    /// Constructor, specific ROI
    InternalModelBin(const BinName& binID,
                     InternalModelPtr Internal,
                     SkyMap<double> *BGMap,
                     std::vector<SkyPos> ROI,
                     bool fitBGModeltoMap = true);

    ///Fits BGModel to BGMap, all TF2 parameters left free
    void FitBackgroundModelToMap(SkyMap<double> *BGMap);

    ///Sets the BG via a (partial) healpix map
    void SetBackgroundFromMap(SkyMap<double> *BGMap);

    ///Returns BG value for a given healpix pixel ID
    double BG(int hp);

    ///Returns free parameter list
    FreeParameterList &GetFreeBackgroundParameterList() { return freeBGParList_; };

    TF2Ptr GetBackgroundModel() { return bgModelBin_; };

    double &CommonNorm() { return intModel_->CommonNorm(); };

    ///Background normalization, multiplied to all BG values (in Map/Model)
    double &BackgroundNorm() { return backgroundNorm_; };

    ///BackgroundNormError
    double &BackgroundNormError() { return backgroundNormError_; };


  private:

    BinName binID_;

    InternalModelPtr intModel_;

    SkyMap<double> *bgMap_;
    std::vector<SkyPos> roiSkyPos_;
    double pixelArea_;

    TF2Ptr bgModelBin_;
    std::map<int, double> bgHash_;

    double backgroundNorm_;
    double backgroundNormError_;

    FreeParameterList freeBGParList_;

    void AddFreeBackgroundParameter(int ParId);

};


#endif
