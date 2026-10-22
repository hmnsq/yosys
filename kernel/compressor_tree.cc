#அந்தர்பாவ "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

நாமாகாஶ Compressor_மரம்
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, முழு width)
{
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec cout = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout, sum);

	SigSpec carry;
	carry.append(State::S0);
	carry.append(cout.extract(0, width - 1));
	திரும்பு {sum, carry};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, முழு width)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->addWire(NEW_ID, width);
	SigSpec cout_h_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout_h_full, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec cin;
	cin.append(State::S0);
	எனில் (width > 1)
		cin.append(cout_h_full.extract(0, width - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec carry_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, s0, d, cin, carry_full, sum);

	SigSpec carry;
	carry.append(State::S0);
	எனில் (width > 1)
		carry.append(carry_full.extract(0, width - 1));

	திரும்பு {sum, carry};
}

std::vector<DepthSig> generate_partial_products(Module *module, SigSpec a, SigSpec b, தருக்க a_signed, தருக்க b_signed, முழு width) {
	முழு width_a = GetSize(a);
	முழு width_b = GetSize(b);
	std::vector<DepthSig> products;
	products.reserve(width_a + 3);

	க்கு (முழு i = 0; i < width_a; i++) {
		SigBit ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(width, பொய்);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, width);
		SigSpec row = module->addWire(NEW_ID, width);
		module->addAnd(NEW_ID, b_shifted, ai_rep, row);

		// Apply Modified Baugh-Wooley inversions for this row
		தருக்க row_is_bottom = (i == width_a - 1);
		தருக்க any_inversion = (row_is_bottom && b_signed) || a_signed;

		எனில் (any_inversion) {
			std::vector<RTLIL::State> mask(width, RTLIL::State::S0);

			க்கு (முழு j = 0; j < width_b; j++) {
				முழு col = i + j;
				எனில் (col < 0 || col >= width)
					தொடர்;
				தருக்க col_is_வலது = (j == width_b - 1);
				// Flip masks
				தருக்க invert = (row_is_bottom && b_signed) ^ (col_is_வலது && a_signed);
				எனில் (invert)
					mask[col] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			தருக்க nonzero = பொய்;
			க்கு (auto s : mask)
				எனில் (s == RTLIL::State::S1) {
					nonzero = உண்மை;
					நிறுத்து;
				}
			எனில் (nonzero) {
				SigSpec inverted = module->addWire(NEW_ID, width);
				module->addXor(NEW_ID, row, SigSpec(RTLIL::Const(mask)), inverted);
				row = inverted;
			}
		}

		products.தள்ளு_back({row, 0});
	}

	// Correction constants
	auto தள்ளு_one_at = [&](முழு col) {
		எனில் (col < 0 || col >= width)
			திரும்பு;
		std::vector<RTLIL::State> v(width, RTLIL::State::S0);
		v[col] = RTLIL::State::S1;
		products.தள்ளு_back({SigSpec(RTLIL::Const(v)), 0});
	};

	எனில் (b_signed)
		தள்ளு_one_at(width_a - 1);
	எனில் (a_signed)
		தள்ளு_one_at(width_b - 1);
	எனில் (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		முழு sign_extension_start = width_a + width_b - 1;
		எனில் (sign_extension_start < width) {
			std::vector<RTLIL::State> v(width, RTLIL::State::S0);
			க்கு (முழு i = sign_extension_start; i < width; i++)
				v[i] = RTLIL::State::S1;
			products.தள்ளு_back({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	திரும்பு products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::vector<DepthSig> operands, முழு width, Strategy strategy, முழு *out_compressor_மொத்தம், முழு *out_final_depth) {
	முழு levels = 0;
	முழு fa_மொத்தம் = 0;
	முழு c42_மொத்தம் = 0;
	முழு final_depth = 0;

	க்கு (auto &op : operands)
		op.sig.extend_u0(width);

	// Only compress operands ready at current level
	க்கு (முழு level = 0; operands.size() > 2; level++) {
		// Partition operands into ready and waiting
		std::vector<DepthSig> ready;
		std::vector<DepthSig> waiting;
		ready.reserve(operands.size());
		க்கு (auto &op : operands) {
			எனில் (op.depth <= level)
				ready.தள்ளு_back(op);
			இல்லையெனில்
				waiting.தள்ளு_back(op);
		}

		எனில் (ready.size() < 3) {
			levels++;
			தொடர்;
		}

		// Apply compressors to ready operands
		std::vector<DepthSig> compressed;
		compressed.reserve(ready.size());
		size_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		தருக்க try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		போது (i < ready.size()) {
			size_t remaining = ready.size() - i;

			எனில் (try_42 && remaining >= 4) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];
				DepthSig d = ready[i + 3];

				auto [sum, carry] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, width);
				முழு dmax = std::max({a.depth, b.depth, c.depth, d.depth});

				compressed.தள்ளு_back({sum, dmax + 2});
				compressed.தள்ளு_back({carry, dmax + 2});

				fa_மொத்தம் += 2;
				c42_மொத்தம் += 1;
				i += 4;
			} இல்லையெனில் எனில் (remaining >= 3) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];

				auto [sum, carry] = emit_compressor_32(module, a.sig, b.sig, c.sig, width);
				முழு dmax = std::max({a.depth, b.depth, c.depth});

				compressed.தள்ளு_back({sum, dmax + 1});
				compressed.தள்ளு_back({carry, dmax + 1});

				fa_மொத்தம் += 1;
				i += 3;
			} இல்லையெனில் {
				// Uncompressed operands pass through to next level
				க்கு (; i < ready.size(); i++)
					compressed.தள்ளு_back(ready[i]);
				நிறுத்து;
			}
		}

		// Merge compressed with waiting operands
		க்கு (auto &op : waiting)
			compressed.தள்ளு_back(op);

		operands = std::move(compressed);
		levels++;
	}

	எனில்(out_compressor_மொத்தம்)
		*out_compressor_மொத்தம் = fa_மொத்தம்;
	எனில் (operands.size() == 0) {
		எனில் (out_final_depth)
			*out_final_depth = 0;
		திரும்பு {SigSpec(State::S0, width), SigSpec(State::S0, width)};
	}
	எனில் (operands.size() == 1) {
		எனில் (out_final_depth)
			*out_final_depth = operands[0].depth;
		திரும்பு {operands[0].sig, SigSpec(State::S0, width)};
	}

	final_depth = std::max(operands[0].depth, operands[1].depth);
	எனில் (out_final_depth)
		*out_final_depth = final_depth;
	log_assert(operands.size() == 2);
	log("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", levels, fa_மொத்தம், c42_மொத்தம், final_depth);
	திரும்பு {operands[0].sig, operands[1].sig};
}

வெறுமை emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	முழு width = GetSize(y);
	log_assert(GetSize(a) == width);
	log_assert(GetSize(b) == width);

	எனில் (width == 0)
		திரும்பு;

	எனில் (width == 1) {
		module->addXorGate(NEW_ID, a[0], b[0], y[0]);
		திரும்பு;
	}

	// Bit level gen and prop
	std::vector<SigBit> g_pre(width), p_pre(width);
	க்கு (முழு i = 0; i < width; i++) {
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
	முழு இலக்கம்_levels = 0;

	போது ((1 << இலக்கம்_levels) < width)
		இலக்கம்_levels++;

	க்கு (முழு k = 1; k <= இலக்கம்_levels; k++) {
		முழு s = 1 << (k - 1);
		std::vector<SigBit> g_next(width), p_next(width);
		க்கு (முழு i = 0; i < width; i++) {
			எனில் (i < s) {
				// Nothing to do
				g_next[i] = g[i];
				p_next[i] = p[i];
			} இல்லையெனில் {
				// g_i^k = g_i | (p_i & g_(i-s))
				SigBit and_pg = module->addWire(NEW_ID);
				module->addAndGate(NEW_ID, p[i], g[i - s], and_pg);
				SigBit gnew = module->addWire(NEW_ID);
				module->addOrGate(NEW_ID, g[i], and_pg, gnew);
				g_next[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				எனில் (k < இலக்கம்_levels) {
					SigBit pnew = module->addWire(NEW_ID);
					module->addAndGate(NEW_ID, p[i], p[i - s], pnew);
					p_next[i] = pnew;
				} இல்லையெனில் {
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
	க்கு (முழு i = 1; i < width; i++)
		module->addXorGate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	தேர்வு (choice) {
		நிலை FinalAdder::DEFAULT:
		நிலை FinalAdder::RIPPLE: {
			திரும்பு module->addAdd(NEW_ID, a, b, y, பொய்);
		}
		நிலை FinalAdder::PARALLEL_PREFIX: {
			emit_kogge_stone(module, a, b, y);
			திரும்பு nullptr;
		}
	}
	log_assert(பொய் && "CompressorTree::emit_final_adder: invalid choice");
	திரும்பு nullptr;
}

FinalAdder pick_final_adder(முழு width, முழு final_depth, FinalMode mode) {
	தேர்வு (mode) {
		நிலை FinalMode::RIPPLE:  திரும்பு FinalAdder::RIPPLE;
		நிலை FinalMode::PREFIX:  திரும்பு FinalAdder::PARALLEL_PREFIX;
		நிலை FinalMode::AUTO:
		இயல்பு: {
			தருக்க wide = width >= RIPPLE_PREFIX_WIDTH_THRESHOLD;
			தருக்க deep = final_depth >= RIPPLE_PREFIX_DEPTH_THRESHOLD;
			திரும்பு (wide && deep) ? FinalAdder::PARALLEL_PREFIX : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_END
