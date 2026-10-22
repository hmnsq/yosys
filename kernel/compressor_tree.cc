#समावेश "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

namespace Compressor_वृक्ष
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, पूर्णांक width)
{
	SigSpec योग = module->addWire(NEW_ID, width);
	SigSpec अ_बाहर = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, अ_बाहर, योग);

	SigSpec carry;
	carry.append(State::S0);
	carry.append(अ_बाहर.extract(0, width - 1));
	वापस {योग, carry};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, पूर्णांक width)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->addWire(NEW_ID, width);
	SigSpec cout_h_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout_h_full, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec अ_अंदर;
	अ_अंदर.append(State::S0);
	अगर (width > 1)
		अ_अंदर.append(cout_h_full.extract(0, width - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec योग = module->addWire(NEW_ID, width);
	SigSpec carry_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, s0, d, अ_अंदर, carry_full, योग);

	SigSpec carry;
	carry.append(State::S0);
	अगर (width > 1)
		carry.append(carry_full.extract(0, width - 1));

	वापस {योग, carry};
}

std::vector<DepthSig> generate_partial_products(Module *module, SigSpec a, SigSpec b, bool a_signed, bool b_signed, पूर्णांक width) {
	पूर्णांक width_a = Get_आकार(a);
	पूर्णांक width_b = Get_आकार(b);
	std::vector<DepthSig> products;
	products.reserve(width_a + 3);

	क्रम (पूर्णांक i = 0; i < width_a; i++) {
		SigBit ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(width, false);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, width);
		SigSpec row = module->addWire(NEW_ID, width);
		module->addAnd(NEW_ID, b_shifted, ai_rep, row);

		// Apply Modified Baugh-Wooley inversions for this row
		bool row_is_bottom = (i == width_a - 1);
		bool any_inversion = (row_is_bottom && b_signed) || a_signed;

		अगर (any_inversion) {
			std::vector<RTLIL::State> mask(width, RTLIL::State::S0);

			क्रम (पूर्णांक j = 0; j < width_b; j++) {
				पूर्णांक col = i + j;
				अगर (col < 0 || col >= width)
					जारी;
				bool col_is_दायाँ = (j == width_b - 1);
				// Flip masks
				bool invert = (row_is_bottom && b_signed) ^ (col_is_दायाँ && a_signed);
				अगर (invert)
					mask[col] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			bool nonzero = false;
			क्रम (स्वतः s : mask)
				अगर (s == RTLIL::State::S1) {
					nonzero = true;
					अवरोध;
				}
			अगर (nonzero) {
				SigSpec inverted = module->addWire(NEW_ID, width);
				module->addXor(NEW_ID, row, SigSpec(RTLIL::Const(mask)), inverted);
				row = inverted;
			}
		}

		products.धकेलो_back({row, 0});
	}

	// Correction constants
	स्वतः धकेलो_one_at = [&](पूर्णांक col) {
		अगर (col < 0 || col >= width)
			वापस;
		std::vector<RTLIL::State> v(width, RTLIL::State::S0);
		v[col] = RTLIL::State::S1;
		products.धकेलो_back({SigSpec(RTLIL::Const(v)), 0});
	};

	अगर (b_signed)
		धकेलो_one_at(width_a - 1);
	अगर (a_signed)
		धकेलो_one_at(width_b - 1);
	अगर (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		पूर्णांक sign_extension_start = width_a + width_b - 1;
		अगर (sign_extension_start < width) {
			std::vector<RTLIL::State> v(width, RTLIL::State::S0);
			क्रम (पूर्णांक i = sign_extension_start; i < width; i++)
				v[i] = RTLIL::State::S1;
			products.धकेलो_back({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	वापस products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::vector<DepthSig> operands, पूर्णांक width, Strategy strategy, पूर्णांक *out_compressor_गिनती, पूर्णांक *out_final_depth) {
	पूर्णांक levels = 0;
	पूर्णांक fa_गिनती = 0;
	पूर्णांक c42_गिनती = 0;
	पूर्णांक final_depth = 0;

	क्रम (स्वतः &op : operands)
		op.sig.extend_u0(width);

	// Only compress operands ready at current level
	क्रम (पूर्णांक level = 0; operands.आकार() > 2; level++) {
		// Partition operands into ready and waiting
		std::vector<DepthSig> ready;
		std::vector<DepthSig> waiting;
		ready.reserve(operands.आकार());
		क्रम (स्वतः &op : operands) {
			अगर (op.depth <= level)
				ready.धकेलो_back(op);
			अन्यथा
				waiting.धकेलो_back(op);
		}

		अगर (ready.आकार() < 3) {
			levels++;
			जारी;
		}

		// Apply compressors to ready operands
		std::vector<DepthSig> compressed;
		compressed.reserve(ready.आकार());
		माप_प्रकार i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		bool try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		जबतक (i < ready.आकार()) {
			माप_प्रकार remaining = ready.आकार() - i;

			अगर (try_42 && remaining >= 4) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];
				DepthSig d = ready[i + 3];

				स्वतः [योग, carry] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, width);
				पूर्णांक dmax = std::max({a.depth, b.depth, c.depth, d.depth});

				compressed.धकेलो_back({योग, dmax + 2});
				compressed.धकेलो_back({carry, dmax + 2});

				fa_गिनती += 2;
				c42_गिनती += 1;
				i += 4;
			} अन्यथा अगर (remaining >= 3) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];

				स्वतः [योग, carry] = emit_compressor_32(module, a.sig, b.sig, c.sig, width);
				पूर्णांक dmax = std::max({a.depth, b.depth, c.depth});

				compressed.धकेलो_back({योग, dmax + 1});
				compressed.धकेलो_back({carry, dmax + 1});

				fa_गिनती += 1;
				i += 3;
			} अन्यथा {
				// Uncompressed operands pass through to next level
				क्रम (; i < ready.आकार(); i++)
					compressed.धकेलो_back(ready[i]);
				अवरोध;
			}
		}

		// Merge compressed with waiting operands
		क्रम (स्वतः &op : waiting)
			compressed.धकेलो_back(op);

		operands = std::move(compressed);
		levels++;
	}

	अगर(out_compressor_गिनती)
		*out_compressor_गिनती = fa_गिनती;
	अगर (operands.आकार() == 0) {
		अगर (out_final_depth)
			*out_final_depth = 0;
		वापस {SigSpec(State::S0, width), SigSpec(State::S0, width)};
	}
	अगर (operands.आकार() == 1) {
		अगर (out_final_depth)
			*out_final_depth = operands[0].depth;
		वापस {operands[0].sig, SigSpec(State::S0, width)};
	}

	final_depth = std::max(operands[0].depth, operands[1].depth);
	अगर (out_final_depth)
		*out_final_depth = final_depth;
	log_assert(operands.आकार() == 2);
	log("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", levels, fa_गिनती, c42_गिनती, final_depth);
	वापस {operands[0].sig, operands[1].sig};
}

व्योम emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	पूर्णांक width = Get_आकार(y);
	log_assert(Get_आकार(a) == width);
	log_assert(Get_आकार(b) == width);

	अगर (width == 0)
		वापस;

	अगर (width == 1) {
		module->addXorGate(NEW_ID, a[0], b[0], y[0]);
		वापस;
	}

	// Bit level gen and prop
	std::vector<SigBit> g_pre(width), p_pre(width);
	क्रम (पूर्णांक i = 0; i < width; i++) {
		SigBit gi = module->addWire(NEW_ID);
		SigBit pi = module->addWire(NEW_ID);
		module->addAndGate(NEW_ID, a[i], b[i], gi);
		module->addXorGate(NEW_ID, a[i], b[i], pi);
		g_pre[i] = gi;
		p_pre[i] = pi;
	}

	// Propagate (g, p) through ceil(log2 W) levels
	std::vector<SigBit> g = g_pre;
	std::vector<SigBit> p = p_pre;
	पूर्णांक अंक_levels = 0;

	जबतक ((1 << अंक_levels) < width)
		अंक_levels++;

	क्रम (पूर्णांक k = 1; k <= अंक_levels; k++) {
		पूर्णांक s = 1 << (k - 1);
		std::vector<SigBit> g_अग्र(width), p_अग्र(width);
		क्रम (पूर्णांक i = 0; i < width; i++) {
			अगर (i < s) {
				// Nothing to do
				g_अग्र[i] = g[i];
				p_अग्र[i] = p[i];
			} अन्यथा {
				// g_i^k = g_i | (p_i & g_(i-s))
				SigBit and_pg = module->addWire(NEW_ID);
				module->addAndGate(NEW_ID, p[i], g[i - s], and_pg);
				SigBit gnew = module->addWire(NEW_ID);
				module->addOrGate(NEW_ID, g[i], and_pg, gnew);
				g_अग्र[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				अगर (k < अंक_levels) {
					SigBit pnew = module->addWire(NEW_ID);
					module->addAndGate(NEW_ID, p[i], p[i - s], pnew);
					p_अग्र[i] = pnew;
				} अन्यथा {
					// Skip last level
					p_अग्र[i] = State::Sx;
				}
			}
		}

		g = std::move(g_अग्र);
		p = std::move(p_अग्र);
	}

	// Sum layer, g[i] is COUT of bit i
	// With CIN 0:
	//   sum[0] = p_pre[0]
	//   sum[i] = p_pre[i] ^ g[i-1] ...
	module->connect(y[0], p_pre[0]);
	क्रम (पूर्णांक i = 1; i < width; i++)
		module->addXorGate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	चयन (choice) {
		हाल FinalAdder::DEFAULT:
		हाल FinalAdder::RIPPLE: {
			वापस module->addAdd(NEW_ID, a, b, y, false);
		}
		हाल FinalAdder::PARALLEL_PREFIX: {
			emit_kogge_stone(module, a, b, y);
			वापस nullptr;
		}
	}
	log_assert(false && "CompressorTree::emit_final_adder: invalid choice");
	वापस nullptr;
}

FinalAdder pick_final_adder(पूर्णांक width, पूर्णांक final_depth, FinalMode mode) {
	चयन (mode) {
		हाल FinalMode::RIPPLE:  वापस FinalAdder::RIPPLE;
		हाल FinalMode::PREFIX:  वापस FinalAdder::PARALLEL_PREFIX;
		हाल FinalMode::AUTO:
		शेष: {
			bool wide = width >= RIPPLE_PREFIX_WIDTH_THRESHOLD;
			bool deep = final_depth >= RIPPLE_PREFIX_DEPTH_THRESHOLD;
			वापस (wide && deep) ? FinalAdder::PARALLEL_PREFIX : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_END
