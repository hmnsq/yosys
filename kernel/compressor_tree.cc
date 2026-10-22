#അന്തര്ഭാവ "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

നാമാകാശ Compressor_വൃക്ഷം
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, പൂർണ്ണസംഖ്യ width)
{
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec cout = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout, sum);

	SigSpec carry;
	carry.append(State::S0);
	carry.append(cout.extract(0, width - 1));
	തിരികെ {sum, carry};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, പൂർണ്ണസംഖ്യ width)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->addWire(NEW_ID, width);
	SigSpec cout_h_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout_h_full, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec cin;
	cin.append(State::S0);
	എങ്കിൽ (width > 1)
		cin.append(cout_h_full.extract(0, width - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec carry_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, s0, d, cin, carry_full, sum);

	SigSpec carry;
	carry.append(State::S0);
	എങ്കിൽ (width > 1)
		carry.append(carry_full.extract(0, width - 1));

	തിരികെ {sum, carry};
}

std::vector<DepthSig> generate_partial_products(Module *module, SigSpec a, SigSpec b, തർക്കം a_signed, തർക്കം b_signed, പൂർണ്ണസംഖ്യ width) {
	പൂർണ്ണസംഖ്യ width_a = GetSize(a);
	പൂർണ്ണസംഖ്യ width_b = GetSize(b);
	std::vector<DepthSig> products;
	products.reserve(width_a + 3);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width_a; i++) {
		SigBit ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(width, അസത്യം);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, width);
		SigSpec row = module->addWire(NEW_ID, width);
		module->addAnd(NEW_ID, b_shifted, ai_rep, row);

		// Apply Modified Baugh-Wooley inversions for this row
		തർക്കം row_is_bottom = (i == width_a - 1);
		തർക്കം any_inversion = (row_is_bottom && b_signed) || a_signed;

		എങ്കിൽ (any_inversion) {
			std::vector<RTLIL::State> mask(width, RTLIL::State::S0);

			വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < width_b; j++) {
				പൂർണ്ണസംഖ്യ col = i + j;
				എങ്കിൽ (col < 0 || col >= width)
					തുടരുക;
				തർക്കം col_is_വലത് = (j == width_b - 1);
				// Flip masks
				തർക്കം invert = (row_is_bottom && b_signed) ^ (col_is_വലത് && a_signed);
				എങ്കിൽ (invert)
					mask[col] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			തർക്കം nonzero = അസത്യം;
			വേണ്ടി (auto s : mask)
				എങ്കിൽ (s == RTLIL::State::S1) {
					nonzero = സത്യം;
					നിർത്തുക;
				}
			എങ്കിൽ (nonzero) {
				SigSpec inverted = module->addWire(NEW_ID, width);
				module->addXor(NEW_ID, row, SigSpec(RTLIL::Const(mask)), inverted);
				row = inverted;
			}
		}

		products.തള്ളുക_back({row, 0});
	}

	// Correction constants
	auto തള്ളുക_one_at = [&](പൂർണ്ണസംഖ്യ col) {
		എങ്കിൽ (col < 0 || col >= width)
			തിരികെ;
		std::vector<RTLIL::State> v(width, RTLIL::State::S0);
		v[col] = RTLIL::State::S1;
		products.തള്ളുക_back({SigSpec(RTLIL::Const(v)), 0});
	};

	എങ്കിൽ (b_signed)
		തള്ളുക_one_at(width_a - 1);
	എങ്കിൽ (a_signed)
		തള്ളുക_one_at(width_b - 1);
	എങ്കിൽ (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		പൂർണ്ണസംഖ്യ sign_extension_start = width_a + width_b - 1;
		എങ്കിൽ (sign_extension_start < width) {
			std::vector<RTLIL::State> v(width, RTLIL::State::S0);
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = sign_extension_start; i < width; i++)
				v[i] = RTLIL::State::S1;
			products.തള്ളുക_back({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	തിരികെ products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::vector<DepthSig> operands, പൂർണ്ണസംഖ്യ width, Strategy strategy, പൂർണ്ണസംഖ്യ *out_compressor_എണ്ണം, പൂർണ്ണസംഖ്യ *out_final_depth) {
	പൂർണ്ണസംഖ്യ levels = 0;
	പൂർണ്ണസംഖ്യ fa_എണ്ണം = 0;
	പൂർണ്ണസംഖ്യ c42_എണ്ണം = 0;
	പൂർണ്ണസംഖ്യ final_depth = 0;

	വേണ്ടി (auto &op : operands)
		op.sig.extend_u0(width);

	// Only compress operands ready at current level
	വേണ്ടി (പൂർണ്ണസംഖ്യ level = 0; operands.size() > 2; level++) {
		// Partition operands into ready and waiting
		std::vector<DepthSig> ready;
		std::vector<DepthSig> waiting;
		ready.reserve(operands.size());
		വേണ്ടി (auto &op : operands) {
			എങ്കിൽ (op.depth <= level)
				ready.തള്ളുക_back(op);
			അല്ലെങ്കിൽ
				waiting.തള്ളുക_back(op);
		}

		എങ്കിൽ (ready.size() < 3) {
			levels++;
			തുടരുക;
		}

		// Apply compressors to ready operands
		std::vector<DepthSig> compressed;
		compressed.reserve(ready.size());
		size_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		തർക്കം try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		ആകുമ്പോൾ (i < ready.size()) {
			size_t remaining = ready.size() - i;

			എങ്കിൽ (try_42 && remaining >= 4) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];
				DepthSig d = ready[i + 3];

				auto [sum, carry] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, width);
				പൂർണ്ണസംഖ്യ dmax = std::max({a.depth, b.depth, c.depth, d.depth});

				compressed.തള്ളുക_back({sum, dmax + 2});
				compressed.തള്ളുക_back({carry, dmax + 2});

				fa_എണ്ണം += 2;
				c42_എണ്ണം += 1;
				i += 4;
			} അല്ലെങ്കിൽ എങ്കിൽ (remaining >= 3) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];

				auto [sum, carry] = emit_compressor_32(module, a.sig, b.sig, c.sig, width);
				പൂർണ്ണസംഖ്യ dmax = std::max({a.depth, b.depth, c.depth});

				compressed.തള്ളുക_back({sum, dmax + 1});
				compressed.തള്ളുക_back({carry, dmax + 1});

				fa_എണ്ണം += 1;
				i += 3;
			} അല്ലെങ്കിൽ {
				// Uncompressed operands pass through to next level
				വേണ്ടി (; i < ready.size(); i++)
					compressed.തള്ളുക_back(ready[i]);
				നിർത്തുക;
			}
		}

		// Merge compressed with waiting operands
		വേണ്ടി (auto &op : waiting)
			compressed.തള്ളുക_back(op);

		operands = std::move(compressed);
		levels++;
	}

	എങ്കിൽ(out_compressor_എണ്ണം)
		*out_compressor_എണ്ണം = fa_എണ്ണം;
	എങ്കിൽ (operands.size() == 0) {
		എങ്കിൽ (out_final_depth)
			*out_final_depth = 0;
		തിരികെ {SigSpec(State::S0, width), SigSpec(State::S0, width)};
	}
	എങ്കിൽ (operands.size() == 1) {
		എങ്കിൽ (out_final_depth)
			*out_final_depth = operands[0].depth;
		തിരികെ {operands[0].sig, SigSpec(State::S0, width)};
	}

	final_depth = std::max(operands[0].depth, operands[1].depth);
	എങ്കിൽ (out_final_depth)
		*out_final_depth = final_depth;
	log_assert(operands.size() == 2);
	log("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", levels, fa_എണ്ണം, c42_എണ്ണം, final_depth);
	തിരികെ {operands[0].sig, operands[1].sig};
}

ശൂന്യം emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	പൂർണ്ണസംഖ്യ width = GetSize(y);
	log_assert(GetSize(a) == width);
	log_assert(GetSize(b) == width);

	എങ്കിൽ (width == 0)
		തിരികെ;

	എങ്കിൽ (width == 1) {
		module->addXorGate(NEW_ID, a[0], b[0], y[0]);
		തിരികെ;
	}

	// Bit level gen and prop
	std::vector<SigBit> g_pre(width), p_pre(width);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++) {
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
	പൂർണ്ണസംഖ്യ അക്കം_levels = 0;

	ആകുമ്പോൾ ((1 << അക്കം_levels) < width)
		അക്കം_levels++;

	വേണ്ടി (പൂർണ്ണസംഖ്യ k = 1; k <= അക്കം_levels; k++) {
		പൂർണ്ണസംഖ്യ s = 1 << (k - 1);
		std::vector<SigBit> g_next(width), p_next(width);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++) {
			എങ്കിൽ (i < s) {
				// Nothing to do
				g_next[i] = g[i];
				p_next[i] = p[i];
			} അല്ലെങ്കിൽ {
				// g_i^k = g_i | (p_i & g_(i-s))
				SigBit and_pg = module->addWire(NEW_ID);
				module->addAndGate(NEW_ID, p[i], g[i - s], and_pg);
				SigBit gnew = module->addWire(NEW_ID);
				module->addOrGate(NEW_ID, g[i], and_pg, gnew);
				g_next[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				എങ്കിൽ (k < അക്കം_levels) {
					SigBit pnew = module->addWire(NEW_ID);
					module->addAndGate(NEW_ID, p[i], p[i - s], pnew);
					p_next[i] = pnew;
				} അല്ലെങ്കിൽ {
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
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < width; i++)
		module->addXorGate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	തിരഞ്ഞെടുപ്പ് (choice) {
		അവസ്ഥ FinalAdder::DEFAULT:
		അവസ്ഥ FinalAdder::RIPPLE: {
			തിരികെ module->addAdd(NEW_ID, a, b, y, അസത്യം);
		}
		അവസ്ഥ FinalAdder::PARALLEL_PREFIX: {
			emit_kogge_stone(module, a, b, y);
			തിരികെ nullptr;
		}
	}
	log_assert(അസത്യം && "CompressorTree::emit_final_adder: invalid choice");
	തിരികെ nullptr;
}

FinalAdder pick_final_adder(പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ final_depth, FinalMode mode) {
	തിരഞ്ഞെടുപ്പ് (mode) {
		അവസ്ഥ FinalMode::RIPPLE:  തിരികെ FinalAdder::RIPPLE;
		അവസ്ഥ FinalMode::PREFIX:  തിരികെ FinalAdder::PARALLEL_PREFIX;
		അവസ്ഥ FinalMode::AUTO:
		സ്ഥിരസ്ഥിതി: {
			തർക്കം wide = width >= RIPPLE_PREFIX_WIDTH_THRESHOLD;
			തർക്കം deep = final_depth >= RIPPLE_PREFIX_DEPTH_THRESHOLD;
			തിരികെ (wide && deep) ? FinalAdder::PARALLEL_PREFIX : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_END
