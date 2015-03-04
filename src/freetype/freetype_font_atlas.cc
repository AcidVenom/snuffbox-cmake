#include "../freetype/freetype_font_atlas.h"
#include "../d3d11/d3d11_texture.h"

#undef max
#undef min

namespace snuffbox
{
  //------------------------------------------------------------------------------------------------
  FontAtlas::FontAtlas(const int& size, const unsigned char& depth) :
    size_(size),
    depth_(depth),
    used_(0)
  {
		if (depth_ == 3)
		{
			depth_ = 4;
		}
    data_.resize(size * size * depth, 0);
    nodes_.emplace_back(1u, 1u, size - 2u);
  }

  //------------------------------------------------------------------------------------------------
	int FontAtlas::TestFit(int idx, const int& width, const int& height)
  {
    const FontAtlasNode& node = nodes_.at(idx);
    int x = node.x;
    int y = node.y;
    int width_left = width;

		if (x + width > size_ - 1)
		{
			return std::numeric_limits<int>::max();
		}

		while (width_left > 0)
    {
      const FontAtlasNode& n = nodes_.at(idx);

			if (n.y > y)
			{
				y = n.y;
			}

			if ((y + height) > (size_ - 1))
			{
				return std::numeric_limits<int>::max();
			}

			width_left -= n.z;
      ++idx;
    }

    return y;
  }

  //------------------------------------------------------------------------------------------------
	FontAtlasRegion FontAtlas::CreateRegion(const int& width, const int& height)
  {
    FontAtlasRegion region(0, 0, width, height);

    int best_height = std::numeric_limits<int>::max();
    int best_width = std::numeric_limits<int>::max();
    int best_index = std::numeric_limits<int>::max();

    for (int i = 0; i < nodes_.size(); ++i)
    {
      int y = TestFit(i, width, height);
      if (y < std::numeric_limits<int>::max())
      {
        const FontAtlasNode& node = nodes_[i];
        if (((y + height) < best_height) ||
					(y + height == best_height && node.z < best_width))
        {
					best_height = y + height;
          best_index = i;
          best_width = node.z;
          region.x = node.x;
          region.y = y;
        }
      }
    }

		if (best_index == std::numeric_limits<int>::max())
    {
      region.x = region.y = std::numeric_limits<int>::max();
      region.width = region.height = 0;
      return region;
    }

    FontAtlasNode node;
    node.x = region.x;
    node.y = region.y + height;
    node.z = width;
		nodes_.insert(nodes_.begin() + best_index, node);

		for (int i = best_index + 1; i < nodes_.size(); ++i)
    {
      FontAtlasNode &node = nodes_[i];
      FontAtlasNode &prev = nodes_[i - 1];

      if (node.x < (prev.x + prev.z))
      {
        int shrink = prev.x + prev.z - node.x;
        node.x += shrink;
        if (node.z <= shrink)
        {
          nodes_.erase(nodes_.begin() + i);
          --i;
        }
        else
        {
          node.z -= shrink;
          break;
        }
      }
      else
        break;
    }

    Merge();

    used_ += width*height;
    return region;
  }

  //-------------------------------------------------------------------------------------------------
  void FontAtlas::Merge()
  {
    for (int i = 0; i < nodes_.size() - 1; ++i)
    {
      FontAtlasNode &node = nodes_[i];
      FontAtlasNode &next = nodes_[i + 1];

      if (node.y == next.y)
      {
        node.z += next.z;
        nodes_.erase(nodes_.begin() + i + 1);
        --i;
      }
    }
  }

  //------------------------------------------------------------------------------------------------
  void FontAtlas::FillRegion(const FontAtlasRegion& region, const unsigned char* data, const int& stride)
  {
    SNUFF_XASSERT(region.x > 0 && region.y > 0 && region.x + region.width <= size_ - 1 && region.y + region.height <= size_ - 1, "The region does not fit in the atlas", "FontAtlas::FillRegion");

		for (int y = 0; y < region.height; ++y)
		{
			for (int x = 0; x < region.width; ++x)
			{
				data_[(region.y + y) * size_ * depth_ + (region.x + x) * depth_ + 0] = data[y*stride + x * 3 + 0];
				data_[(region.y + y) * size_ * depth_ + (region.x + x) * depth_ + 1] = data[y*stride + x * 3 + 1];
				data_[(region.y + y) * size_ * depth_ + (region.x + x) * depth_ + 2] = data[y*stride + x * 3 + 2];
				data_[(region.y + y) * size_ * depth_ + (region.x + x) * depth_ + 3] = 0xff;
			}
		}
  }


  //-------------------------------------------------------------------------------------------------
  void FontAtlas::ClearRegion(const FontAtlasRegion& region)
  {
		for (int y = 0; y < region.height; ++y)
			for (int x = 0; x < region.width; ++x)
				for (int i = 0; i < depth_; ++i)
					data_[(region.y + y) * size_ * depth_ + (region.x + x) * depth_ + i] = 0;
  }

  //------------------------------------------------------------------------------------------------
  void FontAtlas::Clear()
  {
    nodes_.clear();
    nodes_.emplace_back(1u, 1u, size_ - 2u);
  }

  //------------------------------------------------------------------------------------------------
  void FontAtlas::CreateTexture()
  {
		unsigned char* arr = new unsigned char[2048 * 2048 * 4];
		for (unsigned int i = 0; i < data_.size(); ++i)
		{
			arr[i] = data_[i];
		}

		texture_ = AllocatedMemory::Instance().Construct<D3D11Texture>();
		texture_->Create(size_, size_, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM, arr, size_ * depth_);
		texture_->Validate();

		delete arr;
	}

	//------------------------------------------------------------------------------------------------
	const int& FontAtlas::size() const
	{
		return size_;
	}

	//------------------------------------------------------------------------------------------------
	const unsigned char& FontAtlas::depth() const
	{
		return depth_;
	}

	//------------------------------------------------------------------------------------------------
	D3D11Texture* FontAtlas::texture()
	{
		return texture_.get();
	}

  //------------------------------------------------------------------------------------------------
  FontAtlas::~FontAtlas()
  {

  }
}