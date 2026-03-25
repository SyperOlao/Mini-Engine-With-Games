#ifndef PINGPONG_D3D11HELPERS_H
#define PINGPONG_D3D11HELPERS_H

#include <cstdio>
#include <stdexcept>
#include <string>

#include <d3d11.h>

namespace D3d11Helpers
{
    inline void ThrowIfFailed(const HRESULT result, const char *const message)
    {
        if (FAILED(result))
        {
            char hexBuffer[24]{};
            std::snprintf(
                hexBuffer,
                sizeof(hexBuffer),
                "\nHRESULT: 0x%08lX",
                static_cast<unsigned long>(static_cast<unsigned int>(result))
            );
            throw std::runtime_error(std::string(message) + hexBuffer);
        }
    }
}

#endif
