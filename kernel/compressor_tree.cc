#अन्तर्भाव "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

नामाकाश Compressor_रूख
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, पूर्णाङ्क width)
{
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec cout = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout, sum);

	SigSpec carry;
	carry.append(State::S0);
	carry.append(cout.extract(0, width - 1));
	फर्काऊ {sum, carry};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, पूर्णाङ्क width)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->addWire(NEW_ID, width);
	SigSpec cout_h_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout_h_full, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec cin;
	cin.append(State::S0);
	यदि (width > 1)
		cin.append(cout_h_full.extract(0, width - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec carry_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, s0, d, cin, carry_full, sum);

	SigSpec carry;
	carry.append(State::S0);
	यदि (width > 1)
		carry.append(carry_full.extract(0, width - 1));

	फर्काऊ {sum, carry};
}

std::vector<DepthSig> generate_partial_products(Module *module, SigSpec a, SigSpec b, तार्किक a_signed, तार्किक b_signed, पूर्णाङ्क width) {
	पूर्णाङ्क width_a = GetSize(a);
	पूर्णाङ्क width_b = GetSize(b);
	std::vector<DepthSig> products;
	products.reserve(width_a + 3);

	लागि (पूर्णाङ्क i = 0; i < width_a; i++) {
		SigBit ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(width, असत्य);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, width);
		SigSpec row = module->addWire(NEW_ID, width);
		module->addAnd(NEW_ID, b_shifted, ai_rep, row);

		// Apply Modified Baugh-Wooley inversions for this row
		तार्किक row_is_bottom = (i == width_a - 1);
		तार्किक any_inversion = (row_is_bottom && b_signed) || a_signed;

		यदि (any_inversion) {
			std::vector<RTLIL::State> mask(width, RTLIL::State::S0);

			लागि (पूर्णाङ्क j = 0; j < width_b; j++) {
				पूर्णाङ्क col = i + j;
				यदि (col < 0 || col >= width)
					जारी;
				तार्किक col_is_दायाँ = (j == width_b - 1);
				// Flip masks
				तार्किक invert = (row_is_bottom && b_signed) ^ (col_is_दायाँ && a_signed);
				यदि (invert)
					mask[col] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			तार्किक nonzero = असत्य;
			लागि (स्वतः s : mask)
				यदि (s == RTLIL::State::S1) {
					nonzero = सत्य;
					रोक;
				}
			यदि (nonzero) {
				SigSpec inverted = module->addWire(NEW_ID, width);
				module->addXor(NEW_ID, row, SigSpec(RTLIL::Const(mask)), inverted);
				row = inverted;
			}
		}

		products.थप_back({row, 0});
	}

	// Correction constants
	स्वतः थप_one_at = [&](पूर्णाङ्क col) {
		यदि (col < 0 || col >= width)
			फर्काऊ;
		std::vector<RTLIL::State> v(width, RTLIL::State::S0);
		v[col] = RTLIL::State::S1;
		products.थप_back({SigSpec(RTLIL::Const(v)), 0});
	};

	यदि (b_signed)
		थप_one_at(width_a - 1);
	यदि (a_signed)
		थप_one_at(width_b - 1);
	यदि (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		पूर्णाङ्क sign_extension_start = width_a + width_b - 1;
		यदि (sign_extension_start < width) {
			std::vector<RTLIL::State> v(width, RTLIL::State::S0);
			लागि (पूर्णाङ्क i = sign_extension_start; i < width; i++)
				v[i] = RTLIL::State::S1;
			products.थप_back({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	फर्काऊ products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::vector<DepthSig> operands, पूर्णाङ्क width, Strategy strategy, पूर्णाङ्क *out_compressor_गन्ती, पूर्णाङ्क *out_final_depth) {
	पूर्णाङ्क levels = 0;
	पूर्णाङ्क fa_गन्ती = 0;
	पूर्णाङ्क c42_गन्ती = 0;
	पूर्णाङ्क final_depth = 0;

	लागि (स्वतः &op : operands)
		op.sig.extend_u0(width);

	// Only compress operands ready at current level
	लागि (पूर्णाङ्क level = 0; operands.size() > 2; level++) {
		// Partition operands into ready and waiting
		std::vector<DepthSig> ready;
		std::vector<DepthSig> waiting;
		ready.reserve(operands.size());
		लागि (स्वतः &op : operands) {
			यदि (op.depth <= level)
				ready.थप_back(op);
			अन्यथा
				waiting.थप_back(op);
		}

		यदि (ready.size() < 3) {
			levels++;
			जारी;
		}

		// Apply compressors to ready operands
		std::vector<DepthSig> compressed;
		compressed.reserve(ready.size());
		माप_प्रकार i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		तार्किक try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		जबसम्म (i < ready.size()) {
			माप_प्रकार remaining = ready.size() - i;

			यदि (try_42 && remaining >= 4) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];
				DepthSig d = ready[i + 3];

				स्वतः [sum, carry] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, width);
				पूर्णाङ्क dmax = std::max({a.depth, b.depth, c.depth, d.depth});

				compressed.थप_back({sum, dmax + 2});
				compressed.थप_back({carry, dmax + 2});

				fa_गन्ती += 2;
				c42_गन्ती += 1;
				i += 4;
			} अन्यथा यदि (remaining >= 3) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];

				स्वतः [sum, carry] = emit_compressor_32(module, a.sig, b.sig, c.sig, width);
				पूर्णाङ्क dmax = std::max({a.depth, b.depth, c.depth});

				compressed.थप_back({sum, dmax + 1});
				compressed.थप_back({carry, dmax + 1});

				fa_गन्ती += 1;
				i += 3;
			} अन्यथा {
				// Uncompressed operands pass through to next level
				लागि (; i < ready.size(); i++)
					compressed.थप_back(ready[i]);
				रोक;
			}
		}

		// Merge compressed with waiting operands
		लागि (स्वतः &op : waiting)
			compressed.थप_back(op);

		operands = std::move(compressed);
		levels++;
	}

	यदि(out_compressor_गन्ती)
		*out_compressor_गन्ती = fa_गन्ती;
	यदि (operands.size() == 0) {
		यदि (out_final_depth)
			*out_final_depth = 0;
		फर्काऊ {SigSpec(State::S0, width), SigSpec(State::S0, width)};
	}
	यदि (operands.size() == 1) {
		यदि (out_final_depth)
			*out_final_depth = operands[0].depth;
		फर्काऊ {operands[0].sig, SigSpec(State::S0, width)};
	}

	final_depth = std::max(operands[0].depth, operands[1].depth);
	यदि (out_final_depth)
		*out_final_depth = final_depth;
	log_assert(operands.size() == 2);
	log("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", levels, fa_गन्ती, c42_गन्ती, final_depth);
	फर्काऊ {operands[0].sig, operands[1].sig};
}

शून्य emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	पूर्णाङ्क width = GetSize(y);
	log_assert(GetSize(a) == width);
	log_assert(GetSize(b) == width);

	यदि (width == 0)
		फर्काऊ;

	यदि (width == 1) {
		module->addXorGate(NEW_ID, a[0], b[0], y[0]);
		फर्काऊ;
	}

	// Bit level gen and prop
	std::vector<SigBit> g_pre(width), p_pre(width);
	लागि (पूर्णाङ्क i = 0; i < width; i++) {
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
	पूर्णाङ्क अङ्क_levels = 0;

	जबसम्म ((1 << अङ्क_levels) < width)
		अङ्क_levels++;

	लागि (पूर्णाङ्क k = 1; k <= अङ्क_levels; k++) {
		पूर्णाङ्क s = 1 << (k - 1);
		std::vector<SigBit> g_next(width), p_next(width);
		लागि (पूर्णाङ्क i = 0; i < width; i++) {
			यदि (i < s) {
				// Nothing to do
				g_next[i] = g[i];
				p_next[i] = p[i];
			} अन्यथा {
				// g_i^k = g_i | (p_i & g_(i-s))
				SigBit and_pg = module->addWire(NEW_ID);
				module->addAndGate(NEW_ID, p[i], g[i - s], and_pg);
				SigBit gnew = module->addWire(NEW_ID);
				module->addOrGate(NEW_ID, g[i], and_pg, gnew);
				g_next[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				यदि (k < अङ्क_levels) {
					SigBit pnew = module->addWire(NEW_ID);
					module->addAndGate(NEW_ID, p[i], p[i - s], pnew);
					p_next[i] = pnew;
				} अन्यथा {
					// Skip last level
					p_next[i] = State::Sx;
				}
			}
		}

		g = std::move(g_next);
		p = std::move(p_next);
	}

	// Sum layer, g[i] is COUT of bit i
	// With CIN 0:
	//   sum[0] = p_pre[0]
	//   sum[i] = p_pre[i] ^ g[i-1] ...
	module->connect(y[0], p_pre[0]);
	लागि (पूर्णाङ्क i = 1; i < width; i++)
		module->addXorGate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	विकल्प (choice) {
		पक्ष FinalAdder::DEFAULT:
		पक्ष FinalAdder::RIPPLE: {
			फर्काऊ module->addAdd(NEW_ID, a, b, y, असत्य);
		}
		पक्ष FinalAdder::PARALLEL_PREFIX: {
			emit_kogge_stone(module, a, b, y);
			फर्काऊ nullptr;
		}
	}
	log_assert(असत्य && "CompressorTree::emit_final_adder: invalid choice");
	फर्काऊ nullptr;
}

FinalAdder pick_final_adder(पूर्णाङ्क width, पूर्णाङ्क final_depth, FinalMode mode) {
	विकल्प (mode) {
		पक्ष FinalMode::RIPPLE:  फर्काऊ FinalAdder::RIPPLE;
		पक्ष FinalMode::PREFIX:  फर्काऊ FinalAdder::PARALLEL_PREFIX;
		पक्ष FinalMode::AUTO:
		स्वभाव: {
			तार्किक wide = width >= RIPPLE_PREFIX_WIDTH_THRESHOLD;
			तार्किक deep = final_depth >= RIPPLE_PREFIX_DEPTH_THRESHOLD;
			फर्काऊ (wide && deep) ? FinalAdder::PARALLEL_PREFIX : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_END
