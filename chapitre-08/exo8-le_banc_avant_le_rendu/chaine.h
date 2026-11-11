#pragma once
#include <cstdint>

struct ChaineDesc
{
    std::uint32_t imageCount = 0;
};

class ChaineEchange
{
public:
    static constexpr std::uint32_t MaxImages = 4;

    bool Creer(const ChaineDesc &desc) noexcept
    {
        if (desc.imageCount == 0 || desc.imageCount > MaxImages)
            return false;
        mDesc = desc;
        mNextImage = 0;
        mAcquired = false;
        for (std::uint32_t i = 0; i < MaxImages; ++i)
            mHandles[i] = 0;
        return true;
    }

    bool AcquireImage(std::uint32_t &index) noexcept
    {
        if (mDesc.imageCount == 0 || mAcquired)
            return false;
        mAcquired = true;
        index = mNextImage;
        return true;
    }

    bool ReleaseImage() noexcept
    {
        if (!mAcquired)
            return false;
        mAcquired = false;
        mNextImage = (mNextImage + 1u) % mDesc.imageCount;
        return true;
    }

    std::uint32_t GetNextIndex() const noexcept { return mNextImage; }

    std::uint32_t GetLastReleasedIndex() const noexcept
    {
        return (mNextImage + mDesc.imageCount - 1u) % mDesc.imageCount;
    }

    void SetHandle(std::uint32_t index, std::uint64_t handle) noexcept
    {
        if (index < mDesc.imageCount)
            mHandles[index] = handle;
    }

    std::uint64_t GetHandle(std::uint32_t index) const noexcept
    {
        return index < mDesc.imageCount ? mHandles[index] : 0;
    }

private:
    ChaineDesc mDesc;
    std::uint32_t mNextImage = 0;
    bool mAcquired = false;
    std::uint64_t mHandles[MaxImages] = {};
};
