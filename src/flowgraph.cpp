/* SPDX-License-Identifier: MIT */
/**
 * @file flowgraph.cpp
 *
 * Brief description of what's inside this source file.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

/*===========================================================================*\
 * system header files
\*===========================================================================*/
#include <stdio.h>

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include <flowgraph.hpp>

#include <gnuradio/top_block.h>
#include <gnuradio/fft/window.h>
#include <gnuradio/fft/fft_vcc.h>
#include <gnuradio/blocks/file_sink.h>

#include <osmosdr/source.h>

#include <bad/ns_detector.h>
#include <bad/ofdm_fine_frequency_correction.h>
#include <bad/ofdm_sampler.h>
#include <bad/ofdm_coarse_frequency_correction.h>
#include <bad/ofdm_differential_demodulator.h>
#include <bad/frequency_deinterleaver.h>
#include <bad/qpsk_symbol_demapper.h>
#include <bad/ofdm_symbols_selector.h>
#include <bad/fic_decoder.h>
#include <bad/msc_decoder.h>

/*===========================================================================*\
 * 'using namespace' section
\*===========================================================================*/

/*===========================================================================*\
 * preprocessor #define constants and macros
\*===========================================================================*/
#define FLOWGRAPH_FFT_SIZE     2048
#define FLOWGRAPH_SAMPLE_RATE  2048000

/*===========================================================================*\
 * local type definitions
\*===========================================================================*/
namespace
{

} // end of anonymous namespace

/*===========================================================================*\
 * global object definitions
\*===========================================================================*/

/*===========================================================================*\
 * local function declarations
\*===========================================================================*/

/*===========================================================================*\
 * local object definitions
\*===========================================================================*/

/*===========================================================================*\
 * inline function definitions
\*===========================================================================*/

/*===========================================================================*\
 * public function definitions
\*===========================================================================*/
namespace dab
{

flowgraph::flowgraph(const char* filename) :
    m_top_block{gr::make_top_block("GPS-FlowGraph")},
    m_file_sink{gr::blocks::file_sink::make(1, filename)},
    m_fft{gr::fft::fft_vcc::make(FLOWGRAPH_FFT_SIZE, true,
        gr::fft::window::rectangular(FLOWGRAPH_FFT_SIZE), true, 1)},
    m_source{osmosdr::source::make()},
    m_ns_detector{gr::bad::ns_detector::make()},
    m_ofdm_fine_frequency_correction{gr::bad::ofdm_fine_frequency_correction::make()},
    m_ofdm_sampler{gr::bad::ofdm_sampler::make()},
    m_ofdm_coarse_frequency_correction{gr::bad::ofdm_coarse_frequency_correction::make()},
    m_ofdm_differential_demodulator{gr::bad::ofdm_differential_demodulator::make()},
    m_frequency_deinterleaver{gr::bad::frequency_deinterleaver::make()},
    m_qpsk_symbol_demapper{gr::bad::qpsk_symbol_demapper::make()},
    m_ofdm_symbols_selector{gr::bad::ofdm_symbols_selector::make()},
    m_fic_decoder{gr::bad::fic_decoder::make()},
    m_msc_decoder{gr::bad::msc_decoder::make()}
{
    fprintf(stdout, "%s(output file: '%s')\n", __func__, filename);

    m_top_block->connect(m_source, 0, m_ns_detector, 0);
    m_top_block->connect(m_ns_detector, 0, m_ofdm_fine_frequency_correction, 0);
    m_top_block->connect(m_ofdm_fine_frequency_correction, 0, m_ofdm_sampler, 0);
    m_top_block->connect(m_ofdm_sampler, 0, m_fft, 0);
    m_top_block->connect(m_fft, 0, m_ofdm_coarse_frequency_correction, 0);
    m_top_block->connect(m_ofdm_coarse_frequency_correction, 0, m_ofdm_differential_demodulator, 0);
    m_top_block->connect(m_ofdm_differential_demodulator, 0, m_frequency_deinterleaver, 0);
    m_top_block->connect(m_frequency_deinterleaver, 0, m_qpsk_symbol_demapper, 0);
    m_top_block->connect(m_qpsk_symbol_demapper, 0, m_ofdm_symbols_selector, 0);

    m_source->set_sample_rate(FLOWGRAPH_SAMPLE_RATE);
    m_source->set_gain_mode(true);
}

flowgraph::~flowgraph()
{
    stop();
    m_top_block->disconnect_all();
}

void flowgraph::start_fic(uint32_t fHz)
{
    fprintf(stdout, "%s(fHz: %u)\n", __func__, fHz);

    setup_fic_cfg(fHz);

    m_top_block->start();
}

void flowgraph::start_msc_uep(uint32_t fHz, uint32_t subch_start_addr, uint32_t table_index)
{
    fprintf(stdout, "%s(fHz: %u, subch_start_addr: %u, table_index: %u)\n",
        __func__, fHz, subch_start_addr, table_index);

    setup_msc_cfg(fHz);
    m_msc_decoder->set_uep_params(subch_start_addr, table_index);

    m_top_block->start();
}

void flowgraph::start_msc_eep_a(uint32_t fHz, uint32_t subch_start_addr, uint32_t subch_size, uint32_t protection_level)
{
    fprintf(stdout, "%s(fHz: %u, subch_start_addr: %u, subch_size: %u, protection_level: %u)\n",
        __func__, fHz, subch_start_addr, subch_size, protection_level);

    setup_msc_cfg(fHz);
    m_msc_decoder->set_eep_a_params(subch_start_addr, subch_size, protection_level);

    m_top_block->start();
}

void flowgraph::start_msc_eep_b(uint32_t fHz, uint32_t subch_start_addr, uint32_t subch_size, uint32_t protection_level)
{
    fprintf(stdout, "%s(fHz: %u, subch_start_addr: %u, subch_size: %u, protection_level: %u)\n",
        __func__, fHz, subch_start_addr, subch_size, protection_level);

    setup_msc_cfg(fHz);
    m_msc_decoder->set_eep_b_params(subch_start_addr, subch_size, protection_level);

    m_top_block->start();
}

void flowgraph::stop()
{
    fprintf(stdout, "%s()\n", __func__);

    m_top_block->stop();
    m_top_block->wait();
}

} // namespace dab

/*===========================================================================*\
 * protected function definitions
\*===========================================================================*/

/*===========================================================================*\
 * private function definitions
\*===========================================================================*/
namespace dab
{

void flowgraph::setup_fic_cfg(uint32_t fHz)
{
    /* connect blocks */
    m_top_block->connect(m_ofdm_symbols_selector, 0, m_fic_decoder, 0);
    m_top_block->connect(m_fic_decoder, 0, m_file_sink, 0);

    /* set specific configuraton */
    m_source->set_center_freq(fHz);
    m_ofdm_symbols_selector->select_fic_symbols();
}

void flowgraph::setup_msc_cfg(uint32_t fHz)
{
    /* connect blocks */
    m_top_block->connect(m_ofdm_symbols_selector, 0, m_msc_decoder, 0);
    m_top_block->connect(m_msc_decoder, 0, m_file_sink, 0);

    /* set specific configuraton */
    m_source->set_center_freq(fHz);
    m_ofdm_symbols_selector->select_msc_symbols();
}

} // namespace dab

/*===========================================================================*\
 * local function definitions
\*===========================================================================*/

