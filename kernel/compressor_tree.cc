#অন্তর্ভাব "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

নামাকাশ Compressor_বৃক্ষ
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, পূর্ণসংখ্যা width)
{
	SigSpec যোগ = module->addWire(NEW_ID, width);
	SigSpec cout = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout, যোগ);

	SigSpec carry;
	carry.append(State::S0);
	carry.append(cout.extract(0, width - 1));
	ফেরত {যোগ, carry};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, পূর্ণসংখ্যা width)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->addWire(NEW_ID, width);
	SigSpec cout_h_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout_h_full, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec cin;
	cin.append(State::S0);
	যদি (width > 1)
		cin.append(cout_h_full.extract(0, width - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec যোগ = module->addWire(NEW_ID, width);
	SigSpec carry_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, s0, d, cin, carry_full, যোগ);

	SigSpec carry;
	carry.append(State::S0);
	যদি (width > 1)
		carry.append(carry_full.extract(0, width - 1));

	ফেরত {যোগ, carry};
}

std::vector<DepthSig> generate_partial_products(Module *module, SigSpec a, SigSpec b, যৌক্তিক a_signed, যৌক্তিক b_signed, পূর্ণসংখ্যা width) {
	পূর্ণসংখ্যা width_a = Get_আকার(a);
	পূর্ণসংখ্যা width_b = Get_আকার(b);
	std::vector<DepthSig> products;
	products.reserve(width_a + 3);

	জন্য (পূর্ণসংখ্যা i = 0; i < width_a; i++) {
		SigBit ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(width, মিথ্যা);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, width);
		SigSpec row = module->addWire(NEW_ID, width);
		module->addAnd(NEW_ID, b_shifted, ai_rep, row);

		// Apply Modified Baugh-Wooley inversions for this row
		যৌক্তিক row_is_bottom = (i == width_a - 1);
		যৌক্তিক any_inversion = (row_is_bottom && b_signed) || a_signed;

		যদি (any_inversion) {
			std::vector<RTLIL::State> mask(width, RTLIL::State::S0);

			জন্য (পূর্ণসংখ্যা j = 0; j < width_b; j++) {
				পূর্ণসংখ্যা col = i + j;
				যদি (col < 0 || col >= width)
					চালু;
				যৌক্তিক col_is_ডান = (j == width_b - 1);
				// Flip masks
				যৌক্তিক invert = (row_is_bottom && b_signed) ^ (col_is_ডান && a_signed);
				যদি (invert)
					mask[col] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			যৌক্তিক nonzero = মিথ্যা;
			জন্য (auto s : mask)
				যদি (s == RTLIL::State::S1) {
					nonzero = সত্য;
					বিরতি;
				}
			যদি (nonzero) {
				SigSpec inverted = module->addWire(NEW_ID, width);
				module->addXor(NEW_ID, row, SigSpec(RTLIL::Const(mask)), inverted);
				row = inverted;
			}
		}

		products.ঠেলো_back({row, 0});
	}

	// Correction constants
	auto ঠেলো_one_at = [&](পূর্ণসংখ্যা col) {
		যদি (col < 0 || col >= width)
			ফেরত;
		std::vector<RTLIL::State> v(width, RTLIL::State::S0);
		v[col] = RTLIL::State::S1;
		products.ঠেলো_back({SigSpec(RTLIL::Const(v)), 0});
	};

	যদি (b_signed)
		ঠেলো_one_at(width_a - 1);
	যদি (a_signed)
		ঠেলো_one_at(width_b - 1);
	যদি (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		পূর্ণসংখ্যা sign_extension_start = width_a + width_b - 1;
		যদি (sign_extension_start < width) {
			std::vector<RTLIL::State> v(width, RTLIL::State::S0);
			জন্য (পূর্ণসংখ্যা i = sign_extension_start; i < width; i++)
				v[i] = RTLIL::State::S1;
			products.ঠেলো_back({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	ফেরত products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::vector<DepthSig> operands, পূর্ণসংখ্যা width, Strategy strategy, পূর্ণসংখ্যা *out_compressor_গুনতি, পূর্ণসংখ্যা *out_final_depth) {
	পূর্ণসংখ্যা levels = 0;
	পূর্ণসংখ্যা fa_গুনতি = 0;
	পূর্ণসংখ্যা c42_গুনতি = 0;
	পূর্ণসংখ্যা final_depth = 0;

	জন্য (auto &op : operands)
		op.sig.extend_u0(width);

	// Only compress operands ready at current level
	জন্য (পূর্ণসংখ্যা level = 0; operands.আকার_2() > 2; level++) {
		// Partition operands into ready and waiting
		std::vector<DepthSig> ready;
		std::vector<DepthSig> waiting;
		ready.reserve(operands.আকার_2());
		জন্য (auto &op : operands) {
			যদি (op.depth <= level)
				ready.ঠেলো_back(op);
			অন্যথা
				waiting.ঠেলো_back(op);
		}

		যদি (ready.আকার_2() < 3) {
			levels++;
			চালু;
		}

		// Apply compressors to ready operands
		std::vector<DepthSig> compressed;
		compressed.reserve(ready.আকার_2());
		আকার_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		যৌক্তিক try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		যখন (i < ready.আকার_2()) {
			আকার_t remaining = ready.আকার_2() - i;

			যদি (try_42 && remaining >= 4) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];
				DepthSig d = ready[i + 3];

				auto [যোগ, carry] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, width);
				পূর্ণসংখ্যা dmax = std::max({a.depth, b.depth, c.depth, d.depth});

				compressed.ঠেলো_back({যোগ, dmax + 2});
				compressed.ঠেলো_back({carry, dmax + 2});

				fa_গুনতি += 2;
				c42_গুনতি += 1;
				i += 4;
			} অন্যথা যদি (remaining >= 3) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];

				auto [যোগ, carry] = emit_compressor_32(module, a.sig, b.sig, c.sig, width);
				পূর্ণসংখ্যা dmax = std::max({a.depth, b.depth, c.depth});

				compressed.ঠেলো_back({যোগ, dmax + 1});
				compressed.ঠেলো_back({carry, dmax + 1});

				fa_গুনতি += 1;
				i += 3;
			} অন্যথা {
				// Uncompressed operands pass through to next level
				জন্য (; i < ready.আকার_2(); i++)
					compressed.ঠেলো_back(ready[i]);
				বিরতি;
			}
		}

		// Merge compressed with waiting operands
		জন্য (auto &op : waiting)
			compressed.ঠেলো_back(op);

		operands = std::move(compressed);
		levels++;
	}

	যদি(out_compressor_গুনতি)
		*out_compressor_গুনতি = fa_গুনতি;
	যদি (operands.আকার_2() == 0) {
		যদি (out_final_depth)
			*out_final_depth = 0;
		ফেরত {SigSpec(State::S0, width), SigSpec(State::S0, width)};
	}
	যদি (operands.আকার_2() == 1) {
		যদি (out_final_depth)
			*out_final_depth = operands[0].depth;
		ফেরত {operands[0].sig, SigSpec(State::S0, width)};
	}

	final_depth = std::max(operands[0].depth, operands[1].depth);
	যদি (out_final_depth)
		*out_final_depth = final_depth;
	log_assert(operands.আকার_2() == 2);
	log("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", levels, fa_গুনতি, c42_গুনতি, final_depth);
	ফেরত {operands[0].sig, operands[1].sig};
}

শূন্য emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	পূর্ণসংখ্যা width = Get_আকার(y);
	log_assert(Get_আকার(a) == width);
	log_assert(Get_আকার(b) == width);

	যদি (width == 0)
		ফেরত;

	যদি (width == 1) {
		module->addXorGate(NEW_ID, a[0], b[0], y[0]);
		ফেরত;
	}

	// Bit level gen and prop
	std::vector<SigBit> g_pre(width), p_pre(width);
	জন্য (পূর্ণসংখ্যা i = 0; i < width; i++) {
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
	পূর্ণসংখ্যা অঙ্ক_levels = 0;

	যখন ((1 << অঙ্ক_levels) < width)
		অঙ্ক_levels++;

	জন্য (পূর্ণসংখ্যা k = 1; k <= অঙ্ক_levels; k++) {
		পূর্ণসংখ্যা s = 1 << (k - 1);
		std::vector<SigBit> g_পরবর্তী(width), p_পরবর্তী(width);
		জন্য (পূর্ণসংখ্যা i = 0; i < width; i++) {
			যদি (i < s) {
				// Nothing to do
				g_পরবর্তী[i] = g[i];
				p_পরবর্তী[i] = p[i];
			} অন্যথা {
				// g_i^k = g_i | (p_i & g_(i-s))
				SigBit and_pg = module->addWire(NEW_ID);
				module->addAndGate(NEW_ID, p[i], g[i - s], and_pg);
				SigBit gnew = module->addWire(NEW_ID);
				module->addOrGate(NEW_ID, g[i], and_pg, gnew);
				g_পরবর্তী[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				যদি (k < অঙ্ক_levels) {
					SigBit pnew = module->addWire(NEW_ID);
					module->addAndGate(NEW_ID, p[i], p[i - s], pnew);
					p_পরবর্তী[i] = pnew;
				} অন্যথা {
					// Skip last level
					p_পরবর্তী[i] = State::Sx;
				}
			}
		}

		g = std::move(g_পরবর্তী);
		p = std::move(p_পরবর্তী);
	}

	// Sum layer, g[i] is COUT of bit i
	// With CIN 0:
	//   sum[0] = p_pre[0]
	//   sum[i] = p_pre[i] ^ g[i-1] ...
	module->connect(y[0], p_pre[0]);
	জন্য (পূর্ণসংখ্যা i = 1; i < width; i++)
		module->addXorGate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	নির্বাচন (choice) {
		ক্ষেত্র FinalAdder::DEFAULT:
		ক্ষেত্র FinalAdder::RIPPLE: {
			ফেরত module->addAdd(NEW_ID, a, b, y, মিথ্যা);
		}
		ক্ষেত্র FinalAdder::PARALLEL_PREFIX: {
			emit_kogge_stone(module, a, b, y);
			ফেরত nullptr;
		}
	}
	log_assert(মিথ্যা && "CompressorTree::emit_final_adder: invalid choice");
	ফেরত nullptr;
}

FinalAdder pick_final_adder(পূর্ণসংখ্যা width, পূর্ণসংখ্যা final_depth, FinalMode mode) {
	নির্বাচন (mode) {
		ক্ষেত্র FinalMode::RIPPLE:  ফেরত FinalAdder::RIPPLE;
		ক্ষেত্র FinalMode::PREFIX:  ফেরত FinalAdder::PARALLEL_PREFIX;
		ক্ষেত্র FinalMode::AUTO:
		পূর্বনির্ধারিত: {
			যৌক্তিক wide = width >= RIPPLE_PREFIX_WIDTH_THRESHOLD;
			যৌক্তিক deep = final_depth >= RIPPLE_PREFIX_DEPTH_THRESHOLD;
			ফেরত (wide && deep) ? FinalAdder::PARALLEL_PREFIX : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_END
