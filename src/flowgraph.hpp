/* SPDX-License-Identifier: MIT */
/**
 * @file flowgraph.cpp
 *
 * Brief description of what's inside this header file.
 *
 * @author Lukasz Wiecaszek <lukasz.wiecaszek@gmail.com>
 */

#ifndef _FLOWGRAPH_HPP_
#define _FLOWGRAPH_HPP_

/*===========================================================================*\
 * system header files
\*===========================================================================*/

/*===========================================================================*\
 * project header files
\*===========================================================================*/
#include <gnuradio/runtime_types.h>
#include <gnuradio/top_block.h>
#include <gnuradio/blocks/file_sink.h>
#include <gnuradio/fft/window.h>
#include <gnuradio/fft/fft_vcc.h>

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
 * preprocessor #define constants and macros
\*===========================================================================*/

/*===========================================================================*\
 * global type definitions
\*===========================================================================*/
namespace dab
{

class flowgraph
{
public:
    flowgraph(const char* filename);
   ~flowgraph();

    void start_fic(uint32_t fHz);
    void start_msc_uep(uint32_t fHz, uint32_t subch_start_addr, uint32_t table_index);
    void start_msc_eep_a(uint32_t fHz, uint32_t subch_start_addr, uint32_t subch_size, uint32_t protection_level);
    void start_msc_eep_b(uint32_t fHz, uint32_t subch_start_addr, uint32_t subch_size, uint32_t protection_level);

    void stop();

private:
    void setup_fic_cfg(uint32_t fHz);
    void setup_msc_cfg(uint32_t fHz);

    gr::top_block_sptr m_top_block;
    gr::blocks::file_sink::sptr m_file_sink;
    gr::fft::fft_vcc::sptr m_fft;

    osmosdr::source::sptr m_source;

    gr::bad::ns_detector::sptr m_ns_detector;
    gr::bad::ofdm_fine_frequency_correction::sptr m_ofdm_fine_frequency_correction;
    gr::bad::ofdm_sampler::sptr m_ofdm_sampler;
    gr::bad::ofdm_coarse_frequency_correction::sptr m_ofdm_coarse_frequency_correction;
    gr::bad::ofdm_differential_demodulator::sptr m_ofdm_differential_demodulator;
    gr::bad::frequency_deinterleaver::sptr m_frequency_deinterleaver;
    gr::bad::qpsk_symbol_demapper::sptr m_qpsk_symbol_demapper;
    gr::bad::ofdm_symbols_selector::sptr m_ofdm_symbols_selector;
    gr::bad::fic_decoder::sptr m_fic_decoder;
    gr::bad::msc_decoder::sptr m_msc_decoder;
};

} /* end of namespace dab */

/*===========================================================================*\
 * inline function/variable definitions
\*===========================================================================*/
namespace dab
{

} /* end of namespace dab */

/*===========================================================================*\
 * global object declarations
\*===========================================================================*/
namespace dab
{

} /* end of namespace dab */

/*===========================================================================*\
 * function forward declarations
\*===========================================================================*/
namespace dab
{

} /* end of namespace dab */

#endif /* _FLOWGRAPH_HPP_ */
