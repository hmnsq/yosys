/**
 * Generalized compressor-tree utilities for multi-operand addition
 *
 * Terminology:
 * - compressor: $fa viewed as reducing N inputs to M outputs (sum + shifted carry) (N:M compressor)
 * - level:      A stage of parallel compression operations
 * - depth:      Maximum number of N:M compressor levels from any input to a signal
 *
 * Supported compressors:
 * - 3:2 compressor
 * - 4:2 compressor
 *
 * References:
 * - "Some schemes for parallel multipliers" (https://www.acsel-lab.com/arithmetic/arith6/papers/ARITH6_Dadda.pdf)
 * - "Basilisk: Achieving Competitive Performance with Open EDA Tools" (https://arxiv.org/pdf/2405.03523)
 * - "Binary Adder Architectures for Cell-Based VLSI and their Synthesis" (https://iis-people.ee.ethz.ch/~zimmi/publications/adder_arch.pdf)
 * - "A Suggestion for a Fast Multiplier" (https://www.ece.ucdavis.edu/~vojin/CLASSES/EEC280/Web-page/papers/Arithmetic/Wallace_mult.pdf)
 */

#که_نه_تعریف_شوی COMPRESSOR_ونه_H
#تعریف_کړه COMPRESSOR_ونه_H

#ضمن "kernel/sigtools.h"
#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

namespace Compressor_ونه
{

// Width and depth thresholds below which a ripple is preferred over parallel-prefix
// NOTE: Based on "Binary Adder Architectures for Cell-Based VLSI and their Synthesis" (Tables 4.7, 4.9) - the threshold
//       should be the point where Kogge-Stone isn't strictly less efficient than RCA
constexpr صحيح RIPPLE_مختاړی_پلنوالی_پوله = 16;
constexpr صحيح RIPPLE_مختاړی_ژوروالی_پوله = 5;

تعداد class Strategy {
	FA_ONLY,   // 3:2 compressors
	PREFER_42, // Prefer 4:2 grouping when >=4 operands ready
	DADDA,     // Defer compression until column counts exceed
};

بنية ژوروالی_Sig {
	SigSpec sig;
	صحيح ژوروالی;
};

تعداد class FinalAdder {
	DEFAULT,         // emit $add and let downstream techmap pick
	RIPPLE,          // emit $add with explicit narrow hint
	PARALLEL_مختاړی, // emit $add with PARALLEL_PREFIX
};

تعداد class Final_ډېرپېښ {
	AUTO,
	RIPPLE,
	مختاړی,
};

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, صحيح پلنوالی);
std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, صحيح پلنوالی);

/**
 * generate_partial_products() - Generate partial products for FMA concat
 * @module:The Yosys module to which the compressors will be added
 * @a: Signal A
 * @b: Signal B
 * @a_signed: Whether signal A is signed
 * @b_signed: Whether signal B is signed
 * @width: Target width
 *
 * Return: Partial-product matrix as a set of depth-0 vectors
 */
std::ویکټور<ژوروالی_Sig> generate_partial_products(Module *module, SigSpec a, SigSpec b, bool a_signed, bool b_signed, صحيح پلنوالی);

/**
 * reduce_scheduled() - Reduce multiple operands to two using a compressor tree
 * @module: The Yosys module to which the compressors will be added
 * @operands: Vector of operands to be reduced
 * @sigs: Vector of input signals (operands) to be reduced
 * @width: Target bit-width to which all operands will be zero-extended
 * @strategy: Compression strategy to use
 * @out_compressor_count: Optional pointer to return the number of $fa cells emitted
 * @out_final_depth: Optional pointer to return the final depth of the scheduled tree
 *
 * Return: The final two reduced operands, that are to be fed into an adder
 */
std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::ویکټور<ژوروالی_Sig> operands, صحيح پلنوالی, Strategy strategy, صحيح *بهر_compressor_شمېر = nullptr, صحيح *بهر_final_ژوروالی = nullptr);

/**
 * emit_kogge_stone() - Emit a Kogge-Stone parallel-prefix adder
 * @module: The Yosys module to which the gates will be added
 * @a: Signal A
 * @b: Signal B
 * @y: Signal Y = (A + B) mod 2^W
 */
فراغ emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y);

/**
 * emit_final_adder() - Emit the final carry-propagate addition between the two reduced vectors
 * @module:The Yosys module to which the compressors will be added
 * @a: Signal A
 * @b: Signal B
 * @y: Signal Y
 * @choice: Adder type to instantiate
 *
 * Return: Cell* of the emitted instance
 */
Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice);

FinalAdder pick_final_adder(صحيح پلنوالی, صحيح final_ژوروالی, Final_ډېرپېښ ډېرپېښ);

} // namespace CompressorTree

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای // COMPRESSOR_TREE_H
