#include "quadrature_demod.h"
#include <volk/volk.h>
#include "fast_trig.h"

namespace dsp
{
    QuadratureDemodBlock::QuadratureDemodBlock(std::shared_ptr<dsp::stream<complex_t>> input, float gain) : Block(input), gain(gain)
    {
        buffer = (complex_t *)volk_malloc(STREAM_BUFFER_SIZE * sizeof(complex_t), volk_get_alignment());
    }

    QuadratureDemodBlock::~QuadratureDemodBlock()
    {
        volk_free(buffer);
    }

    void QuadratureDemodBlock::work()
    {
        int nsamples = input_stream->read();
        if (nsamples <= 0)
        {
            input_stream->flush();
            return;
        }

        volk_32fc_x2_multiply_conjugate_32fc((lv_32fc_t *)&buffer[0], (lv_32fc_t *)&input_stream->readBuf[1], (lv_32fc_t *)&input_stream->readBuf[0], nsamples);

        for (int i = 0; i < nsamples; i++)
            output_stream->writeBuf[i] = gain * fast_atan2f(buffer[i].imag, buffer[i].real);

        input_stream->flush();
        output_stream->swap(nsamples);
    }
}