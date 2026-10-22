#ଅନ୍ତର୍ଭାଵ "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

ନାମାକାଶ Compressor_ବୃକ୍ଷ
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec cout = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout, sum);

	SigSpec carry;
	carry.append(State::S0);
	carry.append(cout.extract(0, width - 1));
	ଫେରନ୍ତୁ {sum, carry};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->addWire(NEW_ID, width);
	SigSpec cout_h_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout_h_full, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec cin;
	cin.append(State::S0);
	ଯଦି (width > 1)
		cin.append(cout_h_full.extract(0, width - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec carry_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, s0, d, cin, carry_full, sum);

	SigSpec carry;
	carry.append(State::S0);
	ଯଦି (width > 1)
		carry.append(carry_full.extract(0, width - 1));

	ଫେରନ୍ତୁ {sum, carry};
}

std::vector<DepthSig> generate_partial_products(Module *module, SigSpec a, SigSpec b, ତାର୍କିକ a_signed, ତାର୍କିକ b_signed, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width) {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width_a = GetSize(a);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width_b = GetSize(b);
	std::vector<DepthSig> products;
	products.reserve(width_a + 3);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width_a; i++) {
		SigBit ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(width, ମିଥ୍ୟା);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, width);
		SigSpec row = module->addWire(NEW_ID, width);
		module->addAnd(NEW_ID, b_shifted, ai_rep, row);

		// Apply Modified Baugh-Wooley inversions for this row
		ତାର୍କିକ row_is_bottom = (i == width_a - 1);
		ତାର୍କିକ any_inversion = (row_is_bottom && b_signed) || a_signed;

		ଯଦି (any_inversion) {
			std::vector<RTLIL::State> mask(width, RTLIL::State::S0);

			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < width_b; j++) {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = i + j;
				ଯଦି (col < 0 || col >= width)
					ଜାରି;
				ତାର୍କିକ col_is_ଡାହାଣ = (j == width_b - 1);
				// Flip masks
				ତାର୍କିକ invert = (row_is_bottom && b_signed) ^ (col_is_ଡାହାଣ && a_signed);
				ଯଦି (invert)
					mask[col] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			ତାର୍କିକ nonzero = ମିଥ୍ୟା;
			ପାଇଁ (auto s : mask)
				ଯଦି (s == RTLIL::State::S1) {
					nonzero = ସତ୍ୟ;
					ଭାଙ୍ଗ;
				}
			ଯଦି (nonzero) {
				SigSpec inverted = module->addWire(NEW_ID, width);
				module->addXor(NEW_ID, row, SigSpec(RTLIL::Const(mask)), inverted);
				row = inverted;
			}
		}

		products.ଠେଲ_back({row, 0});
	}

	// Correction constants
	auto ଠେଲ_one_at = [&](ପୂର୍ଣ୍ଣସଂଖ୍ୟା col) {
		ଯଦି (col < 0 || col >= width)
			ଫେରନ୍ତୁ;
		std::vector<RTLIL::State> v(width, RTLIL::State::S0);
		v[col] = RTLIL::State::S1;
		products.ଠେଲ_back({SigSpec(RTLIL::Const(v)), 0});
	};

	ଯଦି (b_signed)
		ଠେଲ_one_at(width_a - 1);
	ଯଦି (a_signed)
		ଠେଲ_one_at(width_b - 1);
	ଯଦି (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା sign_extension_start = width_a + width_b - 1;
		ଯଦି (sign_extension_start < width) {
			std::vector<RTLIL::State> v(width, RTLIL::State::S0);
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = sign_extension_start; i < width; i++)
				v[i] = RTLIL::State::S1;
			products.ଠେଲ_back({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	ଫେରନ୍ତୁ products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::vector<DepthSig> operands, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, Strategy strategy, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *out_compressor_ଗଣନା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *out_final_depth) {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା levels = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fa_ଗଣନା = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c42_ଗଣନା = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା final_depth = 0;

	ପାଇଁ (auto &op : operands)
		op.sig.extend_u0(width);

	// Only compress operands ready at current level
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା level = 0; operands.size() > 2; level++) {
		// Partition operands into ready and waiting
		std::vector<DepthSig> ready;
		std::vector<DepthSig> waiting;
		ready.reserve(operands.size());
		ପାଇଁ (auto &op : operands) {
			ଯଦି (op.depth <= level)
				ready.ଠେଲ_back(op);
			ଅନ୍ୟଥା
				waiting.ଠେଲ_back(op);
		}

		ଯଦି (ready.size() < 3) {
			levels++;
			ଜାରି;
		}

		// Apply compressors to ready operands
		std::vector<DepthSig> compressed;
		compressed.reserve(ready.size());
		size_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		ତାର୍କିକ try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		ଯେତେବେଳେ (i < ready.size()) {
			size_t remaining = ready.size() - i;

			ଯଦି (try_42 && remaining >= 4) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];
				DepthSig d = ready[i + 3];

				auto [sum, carry] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, width);
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା dmax = std::max({a.depth, b.depth, c.depth, d.depth});

				compressed.ଠେଲ_back({sum, dmax + 2});
				compressed.ଠେଲ_back({carry, dmax + 2});

				fa_ଗଣନା += 2;
				c42_ଗଣନା += 1;
				i += 4;
			} ଅନ୍ୟଥା ଯଦି (remaining >= 3) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];

				auto [sum, carry] = emit_compressor_32(module, a.sig, b.sig, c.sig, width);
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା dmax = std::max({a.depth, b.depth, c.depth});

				compressed.ଠେଲ_back({sum, dmax + 1});
				compressed.ଠେଲ_back({carry, dmax + 1});

				fa_ଗଣନା += 1;
				i += 3;
			} ଅନ୍ୟଥା {
				// Uncompressed operands pass through to next level
				ପାଇଁ (; i < ready.size(); i++)
					compressed.ଠେଲ_back(ready[i]);
				ଭାଙ୍ଗ;
			}
		}

		// Merge compressed with waiting operands
		ପାଇଁ (auto &op : waiting)
			compressed.ଠେଲ_back(op);

		operands = std::move(compressed);
		levels++;
	}

	ଯଦି(out_compressor_ଗଣନା)
		*out_compressor_ଗଣନା = fa_ଗଣନା;
	ଯଦି (operands.size() == 0) {
		ଯଦି (out_final_depth)
			*out_final_depth = 0;
		ଫେରନ୍ତୁ {SigSpec(State::S0, width), SigSpec(State::S0, width)};
	}
	ଯଦି (operands.size() == 1) {
		ଯଦି (out_final_depth)
			*out_final_depth = operands[0].depth;
		ଫେରନ୍ତୁ {operands[0].sig, SigSpec(State::S0, width)};
	}

	final_depth = std::max(operands[0].depth, operands[1].depth);
	ଯଦି (out_final_depth)
		*out_final_depth = final_depth;
	log_assert(operands.size() == 2);
	log("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", levels, fa_ଗଣନା, c42_ଗଣନା, final_depth);
	ଫେରନ୍ତୁ {operands[0].sig, operands[1].sig};
}

ଶୂନ୍ୟ emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = GetSize(y);
	log_assert(GetSize(a) == width);
	log_assert(GetSize(b) == width);

	ଯଦି (width == 0)
		ଫେରନ୍ତୁ;

	ଯଦି (width == 1) {
		module->addXorGate(NEW_ID, a[0], b[0], y[0]);
		ଫେରନ୍ତୁ;
	}

	// Bit level gen and prop
	std::vector<SigBit> g_pre(width), p_pre(width);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++) {
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_levels = 0;

	ଯେତେବେଳେ ((1 << ଅଙ୍କ_levels) < width)
		ଅଙ୍କ_levels++;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 1; k <= ଅଙ୍କ_levels; k++) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା s = 1 << (k - 1);
		std::vector<SigBit> g_next(width), p_next(width);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++) {
			ଯଦି (i < s) {
				// Nothing to do
				g_next[i] = g[i];
				p_next[i] = p[i];
			} ଅନ୍ୟଥା {
				// g_i^k = g_i | (p_i & g_(i-s))
				SigBit and_pg = module->addWire(NEW_ID);
				module->addAndGate(NEW_ID, p[i], g[i - s], and_pg);
				SigBit gnew = module->addWire(NEW_ID);
				module->addOrGate(NEW_ID, g[i], and_pg, gnew);
				g_next[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				ଯଦି (k < ଅଙ୍କ_levels) {
					SigBit pnew = module->addWire(NEW_ID);
					module->addAndGate(NEW_ID, p[i], p[i - s], pnew);
					p_next[i] = pnew;
				} ଅନ୍ୟଥା {
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
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < width; i++)
		module->addXorGate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	ବାଛ (choice) {
		ସ୍ଥିତି FinalAdder::DEFAULT:
		ସ୍ଥିତି FinalAdder::RIPPLE: {
			ଫେରନ୍ତୁ module->addAdd(NEW_ID, a, b, y, ମିଥ୍ୟା);
		}
		ସ୍ଥିତି FinalAdder::PARALLEL_PREFIX: {
			emit_kogge_stone(module, a, b, y);
			ଫେରନ୍ତୁ nullptr;
		}
	}
	log_assert(ମିଥ୍ୟା && "CompressorTree::emit_final_adder: invalid choice");
	ଫେରନ୍ତୁ nullptr;
}

FinalAdder pick_final_adder(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା final_depth, FinalMode mode) {
	ବାଛ (mode) {
		ସ୍ଥିତି FinalMode::RIPPLE:  ଫେରନ୍ତୁ FinalAdder::RIPPLE;
		ସ୍ଥିତି FinalMode::PREFIX:  ଫେରନ୍ତୁ FinalAdder::PARALLEL_PREFIX;
		ସ୍ଥିତି FinalMode::AUTO:
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
			ତାର୍କିକ wide = width >= RIPPLE_PREFIX_WIDTH_THRESHOLD;
			ତାର୍କିକ deep = final_depth >= RIPPLE_PREFIX_DEPTH_THRESHOLD;
			ଫେରନ୍ତୁ (wide && deep) ? FinalAdder::PARALLEL_PREFIX : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_END
