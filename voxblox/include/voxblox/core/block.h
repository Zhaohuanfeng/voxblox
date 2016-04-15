#ifndef VOXBLOX_CORE_BLOCK_H
#define VOXBLOX_CORE_BLOCK_H

#include "voxblox/core/common.h"
#include "voxblox/core/voxel.h"
#include "voxblox/core/voxel_array.h"

namespace voxblox {

class BaseBlock {
 public:
  typedef std::shared_ptr<BaseBlock> Ptr;
  typedef std::shared_ptr<const BaseBlock> ConstPtr;

  BaseBlock(size_t num_layers, const Coordinates& origin,
            FloatingPoint block_size)
      : num_layers_(num_layers),
        origin_(origin),
        block_size_(block_size),
        has_data_(false) {}
  virtual ~BaseBlock() {}

  inline size_t computeLinearIndexFromVoxelIndex(const VoxelIndex& index,
                                                 size_t vps) const {
    return index.x() + vps * (index.y() + index.z() * vps);
  }

  inline VoxelIndex computeVoxelIndexFromCoordinates(
      const Coordinates& coords, FloatingPoint voxel_size_inv) const {
    return VoxelIndex(static_cast<int>(std::floor((coords.x() - origin_.x()) *
                                                  voxel_size_inv)),
                      static_cast<int>(std::floor((coords.y() - origin_.y()) *
                                                  voxel_size_inv)),
                      static_cast<int>(std::floor((coords.z() - origin_.z()) *
                                                  voxel_size_inv)));
  }

  inline size_t computeLinearIndexFromCoordinates(const Coordinates& coords,
                                                  FloatingPoint voxel_size_inv,
                                                  size_t vps) const {
    return computeLinearIndexFromVoxelIndex(
        computeVoxelIndexFromCoordinates(coords, voxel_size_inv), vps);
  }

  inline Coordinates computeCoordinatesFromLinearIndex(size_t linear_index,
                                                       FloatingPoint voxel_size,
                                                       size_t vps) const {
    return computeCoordinatesFromVoxelIndex(
        computeVoxelIndexFromLinearIndex(linear_index, vps), voxel_size);
  }

  inline Coordinates computeCoordinatesFromVoxelIndex(
      const VoxelIndex& index, FloatingPoint voxel_size) const {
    return index.cast<FloatingPoint>() * voxel_size + origin_;
  }

  inline VoxelIndex computeVoxelIndexFromLinearIndex(size_t linear_index,
                                                     size_t vps) const {
    int rem = linear_index;
    VoxelIndex result;
    std::div_t div_temp = std::div(rem, vps * vps);
    rem = div_temp.rem;
    result.z() = div_temp.quot;
    div_temp = std::div(rem, vps);
    result.y() = div_temp.quot;
    result.x() = div_temp.rem;
    return result;
  }

 protected:
  size_t num_layers_;
  Coordinates origin_;
  FloatingPoint block_size_;
  bool has_data_;
};

class TsdfBlock : BaseBlock {
 public:
  typedef std::shared_ptr<TsdfBlock> Ptr;
  typedef std::shared_ptr<const BaseBlock> ConstPtr;

  TsdfBlock(const Coordinates& origin, size_t voxels_per_side,
            FloatingPoint voxel_size)
      : BaseBlock(1, origin, voxels_per_side * voxel_size),
        has_changed_(false) {
    tsdf_layer_.reset(new VoxelArray<TsdfVoxel>(voxels_per_side, voxel_size));
  }
  virtual ~TsdfBlock() {}

  inline const TsdfVoxel& getTsdfVoxelByLinearIndex(size_t index) const {
    return tsdf_layer_->voxels[index];
  }
  inline const TsdfVoxel& getTsdfVoxelByVoxelIndex(
      const VoxelIndex& index) const {
    return tsdf_layer_->voxels[computeLinearIndexFromVoxelIndex(
        index, tsdf_layer_->voxel_size_inv)];
  }
  inline const TsdfVoxel& getTsdfVoxelByCoordinates(
      const Coordinates& coords) const {
    return tsdf_layer_->voxels[computeLinearIndexFromCoordinates(
        coords, tsdf_layer_->voxel_size_inv, tsdf_layer_->voxels_per_side)];
  }

  inline TsdfVoxel& getTsdfVoxelByLinearIndex(size_t index) {
    return tsdf_layer_->voxels[index];
  }
  inline TsdfVoxel& getTsdfVoxelByVoxelIndex(const VoxelIndex& index) {
    return tsdf_layer_->voxels[computeLinearIndexFromVoxelIndex(
        index, tsdf_layer_->voxel_size_inv)];
  }
  inline TsdfVoxel& getTsdfVoxelByCoordinates(const Coordinates& coords) {
    return tsdf_layer_->voxels[computeLinearIndexFromCoordinates(
        coords, tsdf_layer_->voxel_size_inv, tsdf_layer_->voxels_per_side)];
  }
  inline size_t getNumTsdfVoxels() const { return tsdf_layer_->num_voxels; }

  inline Coordinates getCoordinatesOfTsdfVoxelByLinearIndex(
      size_t index) const {
    return computeCoordinatesFromLinearIndex(index, tsdf_layer_->voxel_size,
                                             tsdf_layer_->voxels_per_side);
  }
  inline Coordinates getCoordinatesOfTsdfVoxelByVoxelIndex(
      const VoxelIndex& index) const {
    return computeCoordinatesFromVoxelIndex(index, tsdf_layer_->voxel_size);
  }

 protected:
  bool has_changed_;

  std::unique_ptr<VoxelArray<TsdfVoxel> > tsdf_layer_;
};

}  // namespace voxblox

#endif  // VOXBLOX_BLOCK_H