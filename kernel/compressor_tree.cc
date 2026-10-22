#include "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

namespace CompressorTree
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, ကိန်းပြည့် width)
{
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec cout = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout, sum);

	SigSpec carry;
	carry.append(State::S0);
	carry.append(cout.extract(0, width - 1));
	ပြန်ပေး {sum, carry};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, ကိန်းပြည့် width)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->addWire(NEW_ID, width);
	SigSpec cout_h_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, a, b, c, cout_h_full, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec cin;
	cin.append(State::S0);
	အကယ်၍ (width > 1)
		cin.append(cout_h_full.extract(0, width - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec sum = module->addWire(NEW_ID, width);
	SigSpec carry_full = module->addWire(NEW_ID, width);
	module->addFa(NEW_ID, s0, d, cin, carry_full, sum);

	SigSpec carry;
	carry.append(State::S0);
	အကယ်၍ (width > 1)
		carry.append(carry_full.extract(0, width - 1));

	ပြန်ပေး {sum, carry};
}

std::vector<DepthSig> generate_partial_products(Module *module, SigSpec a, SigSpec b, bool a_signed, bool b_signed, ကိန်းပြည့် width) {
	ကိန်းပြည့် width_a = GetSize(a);
	ကိန်းပြည့် width_b = GetSize(b);
	std::vector<DepthSig> products;
	products.reserve(width_a + 3);

	အတွက် (ကိန်းပြည့် i = 0; i < width_a; i++) {
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

		အကယ်၍ (any_inversion) {
			std::vector<RTLIL::State> mask(width, RTLIL::State::S0);

			အတွက် (ကိန်းပြည့် j = 0; j < width_b; j++) {
				ကိန်းပြည့် col = i + j;
				အကယ်၍ (col < 0 || col >= width)
					continue;
				bool col_is_right = (j == width_b - 1);
				// Flip masks
				bool invert = (row_is_bottom && b_signed) ^ (col_is_right && a_signed);
				အကယ်၍ (invert)
					mask[col] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			bool nonzero = false;
			အတွက် (auto s : mask)
				အကယ်၍ (s == RTLIL::State::S1) {
					nonzero = true;
					bha_nga;
				}
			အကယ်၍ (nonzero) {
				SigSpec inverted = module->addWire(NEW_ID, width);
				module->addXor(NEW_ID, row, SigSpec(RTLIL::Const(mask)), inverted);
				row = inverted;
			}
		}

		products.push_back({row, 0});
	}

	// Correction constants
	auto push_one_at = [&](ကိန်းပြည့် col) {
		အကယ်၍ (col < 0 || col >= width)
			ပြန်ပေး;
		std::vector<RTLIL::State> v(width, RTLIL::State::S0);
		v[col] = RTLIL::State::S1;
		products.push_back({SigSpec(RTLIL::Const(v)), 0});
	};

	အကယ်၍ (b_signed)
		push_one_at(width_a - 1);
	အကယ်၍ (a_signed)
		push_one_at(width_b - 1);
	အကယ်၍ (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		ကိန်းပြည့် sign_extension_start = width_a + width_b - 1;
		အကယ်၍ (sign_extension_start < width) {
			std::vector<RTLIL::State> v(width, RTLIL::State::S0);
			အတွက် (ကိန်းပြည့် i = sign_extension_start; i < width; i++)
				v[i] = RTLIL::State::S1;
			products.push_back({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	ပြန်ပေး products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::vector<DepthSig> operands, ကိန်းပြည့် width, Strategy strategy, ကိန်းပြည့် *out_compressor_count, ကိန်းပြည့် *out_final_depth) {
	ကိန်းပြည့် levels = 0;
	ကိန်းပြည့် fa_count = 0;
	ကိန်းပြည့် c42_count = 0;
	ကိန်းပြည့် final_depth = 0;

	အတွက် (auto &op : operands)
		op.sig.extend_u0(width);

	// Only compress operands ready at current level
	အတွက် (ကိန်းပြည့် level = 0; operands.size() > 2; level++) {
		// Partition operands into ready and waiting
		std::vector<DepthSig> ready;
		std::vector<DepthSig> waiting;
		ready.reserve(operands.size());
		အတွက် (auto &op : operands) {
			အကယ်၍ (op.depth <= level)
				ready.push_back(op);
			မဟုတ်ပါက
				waiting.push_back(op);
		}

		အကယ်၍ (ready.size() < 3) {
			levels++;
			continue;
		}

		// Apply compressors to ready operands
		std::vector<DepthSig> compressed;
		compressed.reserve(ready.size());
		size_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		bool try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		နေစဉ် (i < ready.size()) {
			size_t remaining = ready.size() - i;

			အကယ်၍ (try_42 && remaining >= 4) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];
				DepthSig d = ready[i + 3];

				auto [sum, carry] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, width);
				ကိန်းပြည့် dmax = std::max({a.depth, b.depth, c.depth, d.depth});

				compressed.push_back({sum, dmax + 2});
				compressed.push_back({carry, dmax + 2});

				fa_count += 2;
				c42_count += 1;
				i += 4;
			} မဟုတ်ပါက အကယ်၍ (remaining >= 3) {
				DepthSig a = ready[i + 0];
				DepthSig b = ready[i + 1];
				DepthSig c = ready[i + 2];

				auto [sum, carry] = emit_compressor_32(module, a.sig, b.sig, c.sig, width);
				ကိန်းပြည့် dmax = std::max({a.depth, b.depth, c.depth});

				compressed.push_back({sum, dmax + 1});
				compressed.push_back({carry, dmax + 1});

				fa_count += 1;
				i += 3;
			} မဟုတ်ပါက {
				// Uncompressed operands pass through to next level
				အတွက် (; i < ready.size(); i++)
					compressed.push_back(ready[i]);
				bha_nga;
			}
		}

		// Merge compressed with waiting operands
		အတွက် (auto &op : waiting)
			compressed.push_back(op);

		operands = std::move(compressed);
		levels++;
	}

	အကယ်၍(out_compressor_count)
		*out_compressor_count = fa_count;
	အကယ်၍ (operands.size() == 0) {
		အကယ်၍ (out_final_depth)
			*out_final_depth = 0;
		ပြန်ပေး {SigSpec(State::S0, width), SigSpec(State::S0, width)};
	}
	အကယ်၍ (operands.size() == 1) {
		အကယ်၍ (out_final_depth)
			*out_final_depth = operands[0].depth;
		ပြန်ပေး {operands[0].sig, SigSpec(State::S0, width)};
	}

	final_depth = std::max(operands[0].depth, operands[1].depth);
	အကယ်၍ (out_final_depth)
		*out_final_depth = final_depth;
	log_assert(operands.size() == 2);
	log("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", levels, fa_count, c42_count, final_depth);
	ပြန်ပေး {operands[0].sig, operands[1].sig};
}

void emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	ကိန်းပြည့် width = GetSize(y);
	log_assert(GetSize(a) == width);
	log_assert(GetSize(b) == width);

	အကယ်၍ (width == 0)
		ပြန်ပေး;

	အကယ်၍ (width == 1) {
		module->addXorGate(NEW_ID, a[0], b[0], y[0]);
		ပြန်ပေး;
	}

	// Bit level gen and prop
	std::vector<SigBit> g_pre(width), p_pre(width);
	အတွက် (ကိန်းပြည့် i = 0; i < width; i++) {
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
	ကိန်းပြည့် num_levels = 0;

	နေစဉ် ((1 << num_levels) < width)
		num_levels++;

	အတွက် (ကိန်းပြည့် k = 1; k <= num_levels; k++) {
		ကိန်းပြည့် s = 1 << (k - 1);
		std::vector<SigBit> g_next(width), p_next(width);
		အတွက် (ကိန်းပြည့် i = 0; i < width; i++) {
			အကယ်၍ (i < s) {
				// Nothing to do
				g_next[i] = g[i];
				p_next[i] = p[i];
			} မဟုတ်ပါက {
				// g_i^k = g_i | (p_i & g_(i-s))
				SigBit and_pg = module->addWire(NEW_ID);
				module->addAndGate(NEW_ID, p[i], g[i - s], and_pg);
				SigBit gnew = module->addWire(NEW_ID);
				module->addOrGate(NEW_ID, g[i], and_pg, gnew);
				g_next[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				အကယ်၍ (k < num_levels) {
					SigBit pnew = module->addWire(NEW_ID);
					module->addAndGate(NEW_ID, p[i], p[i - s], pnew);
					p_next[i] = pnew;
				} မဟုတ်ပါက {
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
	အတွက် (ကိန်းပြည့် i = 1; i < width; i++)
		module->addXorGate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	switch (choice) {
		pak_sa FinalAdder::DEFAULT:
		pak_sa FinalAdder::RIPPLE: {
			ပြန်ပေး module->addAdd(NEW_ID, a, b, y, false);
		}
		pak_sa FinalAdder::PARALLEL_PREFIX: {
			emit_kogge_stone(module, a, b, y);
			ပြန်ပေး nullptr;
		}
	}
	log_assert(false && "CompressorTree::emit_final_adder: invalid choice");
	ပြန်ပေး nullptr;
}

FinalAdder pick_final_adder(ကိန်းပြည့် width, ကိန်းပြည့် final_depth, FinalMode mode) {
	switch (mode) {
		pak_sa FinalMode::RIPPLE:  ပြန်ပေး FinalAdder::RIPPLE;
		pak_sa FinalMode::PREFIX:  ပြန်ပေး FinalAdder::PARALLEL_PREFIX;
		pak_sa FinalMode::AUTO:
		default: {
			bool wide = width >= RIPPLE_PREFIX_WIDTH_THRESHOLD;
			bool deep = final_depth >= RIPPLE_PREFIX_DEPTH_THRESHOLD;
			ပြန်ပေး (wide && deep) ? FinalAdder::PARALLEL_PREFIX : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_END
