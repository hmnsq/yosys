#ಅನ್ತರ್ಭಾವ "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

ನಾಮಾಕಾಶ Compressor_ವೃಕ್ಷ
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, ಪೂರ್ಣಾಙ್ಕ width)
{
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec cout = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout, sum);

	SigSpec carry;
	carry.append(State::S0);
	carry.append(cout.extract(0, width - 1));
	ಪ್ರತ್ಯಾಗಮ {sum, carry};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, ಪೂರ್ಣಾಙ್ಕ width)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->addWire(NEW_ID, width);
	SigSpec cout_h_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout_h_full, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec cin;
	cin.append(State::S0);
	ಯದಿ (width > 1)
		cin.append(cout_h_full.extract(0, width - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec carry_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, s0, d, cin, carry_full, sum);

	SigSpec carry;
	carry.append(State::S0);
	ಯದಿ (width > 1)
		carry.append(carry_full.extract(0, width - 1));

	ಪ್ರತ್ಯಾಗಮ {sum, carry};
}

std::vector<DepthSig> generate_partial_products(Module *module, SigSpec a, SigSpec b, ತಾರ್ಕಿಕ a_signed, ತಾರ್ಕಿಕ b_signed, ಪೂರ್ಣಾಙ್ಕ width) {
	ಪೂರ್ಣಾಙ್ಕ width_a = GetSize(a);
	ಪೂರ್ಣಾಙ್ಕ width_b = GetSize(b);
	std::vector<DepthSig> products;
	products.reserve(width_a + 3);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width_a; i++) {
		SigBit ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(width, ಅಸತ್ಯ);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, width);
		SigSpec row = module->addWire(NEW_ID, width);
		module->addAnd(NEW_ID, b_shifted, ai_rep, row);

		// Apply Modified Baugh-Wooley inversions for this row
		ತಾರ್ಕಿಕ row_is_bottom = (i == width_a - 1);
		ತಾರ್ಕಿಕ any_inversion = (row_is_bottom && b_signed) || a_signed;

		ಯದಿ (any_inversion) {
			std::vector<RTLIL::State> mask(width, RTLIL::State::S0);

			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < width_b; j++) {
				ಪೂರ್ಣಾಙ್ಕ col = i + j;
				ಯದಿ (col < 0 || col >= width)
					ಅನುವರ್ತ;
				ತಾರ್ಕಿಕ col_is_ಬಲ = (j == width_b - 1);
				// Flip masks
				ತಾರ್ಕಿಕ invert = (row_is_bottom && b_signed) ^ (col_is_ಬಲ && a_signed);
				ಯದಿ (invert)
					mask[col] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			ತಾರ್ಕಿಕ nonzero = ಅಸತ್ಯ;
			ಪ್ರತಿ (auto s : mask)
				ಯದಿ (s == RTLIL::State::S1) {
					nonzero = ಸತ್ಯ;
					ಭಙ್ಗ;
				}
			ಯದಿ (nonzero) {
				SigSpec inverted = module->addWire(NEW_ID, width);
				module->addXor(NEW_ID, row, SigSpec(RTLIL::Const(mask)), inverted);
				row = inverted;
			}
		}

		products.ತಳ್ಳು_back({row, 0});
	}

	// Correction constants
	auto ತಳ್ಳು_one_at = [&](ಪೂರ್ಣಾಙ್ಕ col) {
		ಯದಿ (col < 0 || col >= width)
			ಪ್ರತ್ಯಾಗಮ;
		std::vector<RTLIL::State> v(width, RTLIL::State::S0);
		v[col] = RTLIL::State::S1;
		products.ತಳ್ಳು_back({SigSpec(RTLIL::Const(v)), 0});
	};

	ಯದಿ (b_signed)
		ತಳ್ಳು_one_at(width_a - 1);
	ಯದಿ (a_signed)
		ತಳ್ಳು_one_at(width_b - 1);
	ಯದಿ (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		ಪೂರ್ಣಾಙ್ಕ sign_extension_start = width_a + width_b - 1;
		ಯದಿ (sign_extension_start < width) {
			std::vector<RTLIL::State> v(width, RTLIL::State::S0);
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = sign_extension_start; i < width; i++)
				v[i] = RTLIL::State::S1;
			products.ತಳ್ಳು_back({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	ಪ್ರತ್ಯಾಗಮ products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::vector<DepthSig> operands, ಪೂರ್ಣಾಙ್ಕ width, Strategy strategy, ಪೂರ್ಣಾಙ್ಕ *out_compressor_ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ *out_final_depth) {
	ಪೂರ್ಣಾಙ್ಕ levels = 0;
	ಪೂರ್ಣಾಙ್ಕ fa_ಎಣಿಕೆ = 0;
	ಪೂರ್ಣಾಙ್ಕ c42_ಎಣಿಕೆ = 0;
	ಪೂರ್ಣಾಙ್ಕ final_depth = 0;

	ಪ್ರತಿ (auto &op : operands)
		op.sig.extend_u0(width);

	// Only compress operands ready at current level
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ level = 0; operands.size() > 2; level++) {
		// Partition operands into ready and waiting
		std::vector<DepthSig> ready;
		std::vector<DepthSig> waiting;
		ready.reserve(operands.size());
		ಪ್ರತಿ (auto &op : operands) {
			ಯದಿ (op.depth <= level)
				ready.ತಳ್ಳು_back(op);
			ಅನ್ಯಥಾ
				waiting.ತಳ್ಳು_back(op);
		}

		ಯದಿ (ready.size() < 3) {
			levels++;
			ಅನುವರ್ತ;
		}

		// Apply compressors to ready operands
		std::vector<DepthSig> compressed;
		compressed.reserve(ready.size());
		size_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		ತಾರ್ಕಿಕ try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		ಯಾವತ್ (i < ready.size()) {
			size_t remaining = ready.size() - i;

			ಯದಿ (try_42 && remaining >= 4) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];
				DepthSig d = ready[i + 3];

				auto [sum, carry] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, width);
				ಪೂರ್ಣಾಙ್ಕ dmax = std::max({a.depth, b.depth, c.depth, d.depth});

				compressed.ತಳ್ಳು_back({sum, dmax + 2});
				compressed.ತಳ್ಳು_back({carry, dmax + 2});

				fa_ಎಣಿಕೆ += 2;
				c42_ಎಣಿಕೆ += 1;
				i += 4;
			} ಅನ್ಯಥಾ ಯದಿ (remaining >= 3) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];

				auto [sum, carry] = emit_compressor_32(module, a.sig, b.sig, c.sig, width);
				ಪೂರ್ಣಾಙ್ಕ dmax = std::max({a.depth, b.depth, c.depth});

				compressed.ತಳ್ಳು_back({sum, dmax + 1});
				compressed.ತಳ್ಳು_back({carry, dmax + 1});

				fa_ಎಣಿಕೆ += 1;
				i += 3;
			} ಅನ್ಯಥಾ {
				// Uncompressed operands pass through to next level
				ಪ್ರತಿ (; i < ready.size(); i++)
					compressed.ತಳ್ಳು_back(ready[i]);
				ಭಙ್ಗ;
			}
		}

		// Merge compressed with waiting operands
		ಪ್ರತಿ (auto &op : waiting)
			compressed.ತಳ್ಳು_back(op);

		operands = std::move(compressed);
		levels++;
	}

	ಯದಿ(out_compressor_ಎಣಿಕೆ)
		*out_compressor_ಎಣಿಕೆ = fa_ಎಣಿಕೆ;
	ಯದಿ (operands.size() == 0) {
		ಯದಿ (out_final_depth)
			*out_final_depth = 0;
		ಪ್ರತ್ಯಾಗಮ {SigSpec(State::S0, width), SigSpec(State::S0, width)};
	}
	ಯದಿ (operands.size() == 1) {
		ಯದಿ (out_final_depth)
			*out_final_depth = operands[0].depth;
		ಪ್ರತ್ಯಾಗಮ {operands[0].sig, SigSpec(State::S0, width)};
	}

	final_depth = std::max(operands[0].depth, operands[1].depth);
	ಯದಿ (out_final_depth)
		*out_final_depth = final_depth;
	log_assert(operands.size() == 2);
	log("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", levels, fa_ಎಣಿಕೆ, c42_ಎಣಿಕೆ, final_depth);
	ಪ್ರತ್ಯಾಗಮ {operands[0].sig, operands[1].sig};
}

ಶೂನ್ಯ emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	ಪೂರ್ಣಾಙ್ಕ width = GetSize(y);
	log_assert(GetSize(a) == width);
	log_assert(GetSize(b) == width);

	ಯದಿ (width == 0)
		ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (width == 1) {
		module->addXorGate(NEW_ID, a[0], b[0], y[0]);
		ಪ್ರತ್ಯಾಗಮ;
	}

	// Bit level gen and prop
	std::vector<SigBit> g_pre(width), p_pre(width);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++) {
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
	ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_levels = 0;

	ಯಾವತ್ ((1 << ಅಂಕೆ_levels) < width)
		ಅಂಕೆ_levels++;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 1; k <= ಅಂಕೆ_levels; k++) {
		ಪೂರ್ಣಾಙ್ಕ s = 1 << (k - 1);
		std::vector<SigBit> g_next(width), p_next(width);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++) {
			ಯದಿ (i < s) {
				// Nothing to do
				g_next[i] = g[i];
				p_next[i] = p[i];
			} ಅನ್ಯಥಾ {
				// g_i^k = g_i | (p_i & g_(i-s))
				SigBit and_pg = module->addWire(NEW_ID);
				module->addAndGate(NEW_ID, p[i], g[i - s], and_pg);
				SigBit gnew = module->addWire(NEW_ID);
				module->addOrGate(NEW_ID, g[i], and_pg, gnew);
				g_next[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				ಯದಿ (k < ಅಂಕೆ_levels) {
					SigBit pnew = module->addWire(NEW_ID);
					module->addAndGate(NEW_ID, p[i], p[i - s], pnew);
					p_next[i] = pnew;
				} ಅನ್ಯಥಾ {
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
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < width; i++)
		module->addXorGate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	ವಿಕಲ್ಪ (choice) {
		ಪಕ್ಷ FinalAdder::DEFAULT:
		ಪಕ್ಷ FinalAdder::RIPPLE: {
			ಪ್ರತ್ಯಾಗಮ module->addAdd(NEW_ID, a, b, y, ಅಸತ್ಯ);
		}
		ಪಕ್ಷ FinalAdder::PARALLEL_PREFIX: {
			emit_kogge_stone(module, a, b, y);
			ಪ್ರತ್ಯಾಗಮ nullptr;
		}
	}
	log_assert(ಅಸತ್ಯ && "CompressorTree::emit_final_adder: invalid choice");
	ಪ್ರತ್ಯಾಗಮ nullptr;
}

FinalAdder pick_final_adder(ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ final_depth, FinalMode mode) {
	ವಿಕಲ್ಪ (mode) {
		ಪಕ್ಷ FinalMode::RIPPLE:  ಪ್ರತ್ಯಾಗಮ FinalAdder::RIPPLE;
		ಪಕ್ಷ FinalMode::PREFIX:  ಪ್ರತ್ಯಾಗಮ FinalAdder::PARALLEL_PREFIX;
		ಪಕ್ಷ FinalMode::AUTO:
		ಸ್ವಭಾವ: {
			ತಾರ್ಕಿಕ wide = width >= RIPPLE_PREFIX_WIDTH_THRESHOLD;
			ತಾರ್ಕಿಕ deep = final_depth >= RIPPLE_PREFIX_DEPTH_THRESHOLD;
			ಪ್ರತ್ಯಾಗಮ (wide && deep) ? FinalAdder::PARALLEL_PREFIX : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_END
