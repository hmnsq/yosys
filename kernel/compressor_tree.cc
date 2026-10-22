#شامل "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

فضاینام Compressor_درخت
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, صحیح width)
{
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec cout = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout, sum);

	SigSpec carry;
	carry.append(State::S0);
	carry.append(cout.extract(0, width - 1));
	بازگشت {sum, carry};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, صحیح width)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->addWire(NEW_ID, width);
	SigSpec cout_h_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout_h_full, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec cin;
	cin.append(State::S0);
	اگر (width > 1)
		cin.append(cout_h_full.extract(0, width - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec carry_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, s0, d, cin, carry_full, sum);

	SigSpec carry;
	carry.append(State::S0);
	اگر (width > 1)
		carry.append(carry_full.extract(0, width - 1));

	بازگشت {sum, carry};
}

std::vector<DepthSig> generate_partial_products(Module *module, SigSpec a, SigSpec b, منطقی a_signed, منطقی b_signed, صحیح width) {
	صحیح width_a = GetSize(a);
	صحیح width_b = GetSize(b);
	std::vector<DepthSig> products;
	products.reserve(width_a + 3);

	برای (صحیح i = 0; i < width_a; i++) {
		SigBit ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(width, نادرست);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, width);
		SigSpec row = module->addWire(NEW_ID, width);
		module->addAnd(NEW_ID, b_shifted, ai_rep, row);

		// Apply Modified Baugh-Wooley inversions for this row
		منطقی row_is_bottom = (i == width_a - 1);
		منطقی any_inversion = (row_is_bottom && b_signed) || a_signed;

		اگر (any_inversion) {
			std::vector<RTLIL::State> mask(width, RTLIL::State::S0);

			برای (صحیح j = 0; j < width_b; j++) {
				صحیح col = i + j;
				اگر (col < 0 || col >= width)
					ادامه;
				منطقی col_is_راست = (j == width_b - 1);
				// Flip masks
				منطقی invert = (row_is_bottom && b_signed) ^ (col_is_راست && a_signed);
				اگر (invert)
					mask[col] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			منطقی nonzero = نادرست;
			برای (auto s : mask)
				اگر (s == RTLIL::State::S1) {
					nonzero = درست;
					بشکن;
				}
			اگر (nonzero) {
				SigSpec inverted = module->addWire(NEW_ID, width);
				module->addXor(NEW_ID, row, SigSpec(RTLIL::Const(mask)), inverted);
				row = inverted;
			}
		}

		products.افزودن_back({row, 0});
	}

	// Correction constants
	auto افزودن_one_at = [&](صحیح col) {
		اگر (col < 0 || col >= width)
			بازگشت;
		std::vector<RTLIL::State> v(width, RTLIL::State::S0);
		v[col] = RTLIL::State::S1;
		products.افزودن_back({SigSpec(RTLIL::Const(v)), 0});
	};

	اگر (b_signed)
		افزودن_one_at(width_a - 1);
	اگر (a_signed)
		افزودن_one_at(width_b - 1);
	اگر (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		صحیح sign_extension_start = width_a + width_b - 1;
		اگر (sign_extension_start < width) {
			std::vector<RTLIL::State> v(width, RTLIL::State::S0);
			برای (صحیح i = sign_extension_start; i < width; i++)
				v[i] = RTLIL::State::S1;
			products.افزودن_back({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	بازگشت products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::vector<DepthSig> operands, صحیح width, Strategy strategy, صحیح *out_compressor_شمار, صحیح *out_final_depth) {
	صحیح levels = 0;
	صحیح fa_شمار = 0;
	صحیح c42_شمار = 0;
	صحیح final_depth = 0;

	برای (auto &op : operands)
		op.sig.extend_u0(width);

	// Only compress operands ready at current level
	برای (صحیح level = 0; operands.size() > 2; level++) {
		// Partition operands into ready and waiting
		std::vector<DepthSig> ready;
		std::vector<DepthSig> waiting;
		ready.reserve(operands.size());
		برای (auto &op : operands) {
			اگر (op.depth <= level)
				ready.افزودن_back(op);
			وگرنه
				waiting.افزودن_back(op);
		}

		اگر (ready.size() < 3) {
			levels++;
			ادامه;
		}

		// Apply compressors to ready operands
		std::vector<DepthSig> compressed;
		compressed.reserve(ready.size());
		size_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		منطقی try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		وقتی (i < ready.size()) {
			size_t remaining = ready.size() - i;

			اگر (try_42 && remaining >= 4) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];
				DepthSig d = ready[i + 3];

				auto [sum, carry] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, width);
				صحیح dmax = std::max({a.depth, b.depth, c.depth, d.depth});

				compressed.افزودن_back({sum, dmax + 2});
				compressed.افزودن_back({carry, dmax + 2});

				fa_شمار += 2;
				c42_شمار += 1;
				i += 4;
			} وگرنه اگر (remaining >= 3) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];

				auto [sum, carry] = emit_compressor_32(module, a.sig, b.sig, c.sig, width);
				صحیح dmax = std::max({a.depth, b.depth, c.depth});

				compressed.افزودن_back({sum, dmax + 1});
				compressed.افزودن_back({carry, dmax + 1});

				fa_شمار += 1;
				i += 3;
			} وگرنه {
				// Uncompressed operands pass through to next level
				برای (; i < ready.size(); i++)
					compressed.افزودن_back(ready[i]);
				بشکن;
			}
		}

		// Merge compressed with waiting operands
		برای (auto &op : waiting)
			compressed.افزودن_back(op);

		operands = std::move(compressed);
		levels++;
	}

	اگر(out_compressor_شمار)
		*out_compressor_شمار = fa_شمار;
	اگر (operands.size() == 0) {
		اگر (out_final_depth)
			*out_final_depth = 0;
		بازگشت {SigSpec(State::S0, width), SigSpec(State::S0, width)};
	}
	اگر (operands.size() == 1) {
		اگر (out_final_depth)
			*out_final_depth = operands[0].depth;
		بازگشت {operands[0].sig, SigSpec(State::S0, width)};
	}

	final_depth = std::max(operands[0].depth, operands[1].depth);
	اگر (out_final_depth)
		*out_final_depth = final_depth;
	log_assert(operands.size() == 2);
	log("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", levels, fa_شمار, c42_شمار, final_depth);
	بازگشت {operands[0].sig, operands[1].sig};
}

تهی emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	صحیح width = GetSize(y);
	log_assert(GetSize(a) == width);
	log_assert(GetSize(b) == width);

	اگر (width == 0)
		بازگشت;

	اگر (width == 1) {
		module->addXorGate(NEW_ID, a[0], b[0], y[0]);
		بازگشت;
	}

	// Bit level gen and prop
	std::vector<SigBit> g_pre(width), p_pre(width);
	برای (صحیح i = 0; i < width; i++) {
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
	صحیح رقم_levels = 0;

	وقتی ((1 << رقم_levels) < width)
		رقم_levels++;

	برای (صحیح k = 1; k <= رقم_levels; k++) {
		صحیح s = 1 << (k - 1);
		std::vector<SigBit> g_next(width), p_next(width);
		برای (صحیح i = 0; i < width; i++) {
			اگر (i < s) {
				// Nothing to do
				g_next[i] = g[i];
				p_next[i] = p[i];
			} وگرنه {
				// g_i^k = g_i | (p_i & g_(i-s))
				SigBit and_pg = module->addWire(NEW_ID);
				module->addAndGate(NEW_ID, p[i], g[i - s], and_pg);
				SigBit gnew = module->addWire(NEW_ID);
				module->addOrGate(NEW_ID, g[i], and_pg, gnew);
				g_next[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				اگر (k < رقم_levels) {
					SigBit pnew = module->addWire(NEW_ID);
					module->addAndGate(NEW_ID, p[i], p[i - s], pnew);
					p_next[i] = pnew;
				} وگرنه {
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
	برای (صحیح i = 1; i < width; i++)
		module->addXorGate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	انتخاب (choice) {
		حالت FinalAdder::DEFAULT:
		حالت FinalAdder::RIPPLE: {
			بازگشت module->addAdd(NEW_ID, a, b, y, نادرست);
		}
		حالت FinalAdder::PARALLEL_PREFIX: {
			emit_kogge_stone(module, a, b, y);
			بازگشت nullptr;
		}
	}
	log_assert(نادرست && "CompressorTree::emit_final_adder: invalid choice");
	بازگشت nullptr;
}

FinalAdder pick_final_adder(صحیح width, صحیح final_depth, FinalMode mode) {
	انتخاب (mode) {
		حالت FinalMode::RIPPLE:  بازگشت FinalAdder::RIPPLE;
		حالت FinalMode::PREFIX:  بازگشت FinalAdder::PARALLEL_PREFIX;
		حالت FinalMode::AUTO:
		پیشفرض: {
			منطقی wide = width >= RIPPLE_PREFIX_WIDTH_THRESHOLD;
			منطقی deep = final_depth >= RIPPLE_PREFIX_DEPTH_THRESHOLD;
			بازگشت (wide && deep) ? FinalAdder::PARALLEL_PREFIX : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_END
