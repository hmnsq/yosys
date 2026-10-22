#ضمن "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

نطاق Compressor_درخت
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, صحيح عرض)
{
	SigSpec جمع = module->جمع_کن_Wire(NEW_ID, عرض);
	SigSpec خروجی = module->جمع_کن_Wire(NEW_ID, عرض);
	module->جمع_کن_Fa(NEW_ID, a, b, c, خروجی, جمع);

	SigSpec انتقال;
	انتقال.append(State::S0);
	انتقال.append(خروجی.استخراج(0, عرض - 1));
	أرجع {جمع, انتقال};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, صحيح عرض)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->جمع_کن_Wire(NEW_ID, عرض);
	SigSpec cout_h_پر = module->جمع_کن_Wire(NEW_ID, عرض);
	module->جمع_کن_Fa(NEW_ID, a, b, c, cout_h_پر, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec ورودی;
	ورودی.append(State::S0);
	إذا (عرض > 1)
		ورودی.append(cout_h_پر.استخراج(0, عرض - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec جمع = module->جمع_کن_Wire(NEW_ID, عرض);
	SigSpec انتقال_پر = module->جمع_کن_Wire(NEW_ID, عرض);
	module->جمع_کن_Fa(NEW_ID, s0, d, ورودی, انتقال_پر, جمع);

	SigSpec انتقال;
	انتقال.append(State::S0);
	إذا (عرض > 1)
		انتقال.append(انتقال_پر.استخراج(0, عرض - 1));

	أرجع {جمع, انتقال};
}

std::وکتور<عمق_Sig> generate_partial_products(Module *module, SigSpec a, SigSpec b, منطقي a_signed, منطقي b_signed, صحيح عرض) {
	صحيح عرض_a = Get_اندازه(a);
	صحيح عرض_b = Get_اندازه(b);
	std::وکتور<عمق_Sig> products;
	products.reserve(عرض_a + 3);

	لكل (صحيح i = 0; i < عرض_a; i++) {
		Sig_بیت ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(عرض, خطأ);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, عرض);
		SigSpec ردیف = module->جمع_کن_Wire(NEW_ID, عرض);
		module->جمع_کن_And(NEW_ID, b_shifted, ai_rep, ردیف);

		// Apply Modified Baugh-Wooley inversions for this row
		منطقي ردیف_is_bottom = (i == عرض_a - 1);
		منطقي any_inversion = (ردیف_is_bottom && b_signed) || a_signed;

		إذا (any_inversion) {
			std::وکتور<RTLIL::State> ماسک(عرض, RTLIL::State::S0);

			لكل (صحيح j = 0; j < عرض_b; j++) {
				صحيح ستون = i + j;
				إذا (ستون < 0 || ستون >= عرض)
					استمر;
				منطقي ستون_is_راست = (j == عرض_b - 1);
				// Flip masks
				منطقي invert = (ردیف_is_bottom && b_signed) ^ (ستون_is_راست && a_signed);
				إذا (invert)
					ماسک[ستون] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			منطقي nonzero = خطأ;
			لكل (خودکار s : ماسک)
				إذا (s == RTLIL::State::S1) {
					nonzero = درست;
					اكسر;
				}
			إذا (nonzero) {
				SigSpec inverted = module->جمع_کن_Wire(NEW_ID, عرض);
				module->جمع_کن_Xor(NEW_ID, ردیف, SigSpec(RTLIL::Const(ماسک)), inverted);
				ردیف = inverted;
			}
		}

		products.افزودن_پشت({ردیف, 0});
	}

	// Correction constants
	خودکار افزودن_one_at = [&](صحيح ستون) {
		إذا (ستون < 0 || ستون >= عرض)
			أرجع;
		std::وکتور<RTLIL::State> v(عرض, RTLIL::State::S0);
		v[ستون] = RTLIL::State::S1;
		products.افزودن_پشت({SigSpec(RTLIL::Const(v)), 0});
	};

	إذا (b_signed)
		افزودن_one_at(عرض_a - 1);
	إذا (a_signed)
		افزودن_one_at(عرض_b - 1);
	إذا (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		صحيح sign_extension_شروع = عرض_a + عرض_b - 1;
		إذا (sign_extension_شروع < عرض) {
			std::وکتور<RTLIL::State> v(عرض, RTLIL::State::S0);
			لكل (صحيح i = sign_extension_شروع; i < عرض; i++)
				v[i] = RTLIL::State::S1;
			products.افزودن_پشت({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	أرجع products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::وکتور<عمق_Sig> operands, صحيح عرض, Strategy strategy, صحيح *بیرون_compressor_شمار, صحيح *بیرون_final_عمق) {
	صحيح سطوح = 0;
	صحيح fa_شمار = 0;
	صحيح c42_شمار = 0;
	صحيح final_عمق = 0;

	لكل (خودکار &op : operands)
		op.sig.extend_u0(عرض);

	// Only compress operands ready at current level
	لكل (صحيح level = 0; operands.اندازه() > 2; level++) {
		// Partition operands into ready and waiting
		std::وکتور<عمق_Sig> ready;
		std::وکتور<عمق_Sig> انتظار;
		ready.reserve(operands.اندازه());
		لكل (خودکار &op : operands) {
			إذا (op.عمق <= level)
				ready.افزودن_پشت(op);
			وإلا
				انتظار.افزودن_پشت(op);
		}

		إذا (ready.اندازه() < 3) {
			سطوح++;
			استمر;
		}

		// Apply compressors to ready operands
		std::وکتور<عمق_Sig> compressed;
		compressed.reserve(ready.اندازه());
		اندازه_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		منطقي try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		طالما (i < ready.اندازه()) {
			اندازه_t مانده = ready.اندازه() - i;

			إذا (try_42 && مانده >= 4) {
				عمق_Sig a = ready[i + 0];
				عمق_Sig b = ready[i + 1];
				عمق_Sig c = ready[i + 2];
				عمق_Sig d = ready[i + 3];

				خودکار [جمع, انتقال] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, عرض);
				صحيح dmax = std::max({a.عمق, b.عمق, c.عمق, d.عمق});

				compressed.افزودن_پشت({جمع, dmax + 2});
				compressed.افزودن_پشت({انتقال, dmax + 2});

				fa_شمار += 2;
				c42_شمار += 1;
				i += 4;
			} وإلا إذا (مانده >= 3) {
				عمق_Sig a = ready[i + 0];
				عمق_Sig b = ready[i + 1];
				عمق_Sig c = ready[i + 2];

				خودکار [جمع, انتقال] = emit_compressor_32(module, a.sig, b.sig, c.sig, عرض);
				صحيح dmax = std::max({a.عمق, b.عمق, c.عمق});

				compressed.افزودن_پشت({جمع, dmax + 1});
				compressed.افزودن_پشت({انتقال, dmax + 1});

				fa_شمار += 1;
				i += 3;
			} وإلا {
				// Uncompressed operands pass through to next level
				لكل (; i < ready.اندازه(); i++)
					compressed.افزودن_پشت(ready[i]);
				اكسر;
			}
		}

		// Merge compressed with waiting operands
		لكل (خودکار &op : انتظار)
			compressed.افزودن_پشت(op);

		operands = std::move(compressed);
		سطوح++;
	}

	إذا(بیرون_compressor_شمار)
		*بیرون_compressor_شمار = fa_شمار;
	إذا (operands.اندازه() == 0) {
		إذا (بیرون_final_عمق)
			*بیرون_final_عمق = 0;
		أرجع {SigSpec(State::S0, عرض), SigSpec(State::S0, عرض)};
	}
	إذا (operands.اندازه() == 1) {
		إذا (بیرون_final_عمق)
			*بیرون_final_عمق = operands[0].عمق;
		أرجع {operands[0].sig, SigSpec(State::S0, عرض)};
	}

	final_عمق = std::max(operands[0].عمق, operands[1].عمق);
	إذا (بیرون_final_عمق)
		*بیرون_final_عمق = final_عمق;
	log_assert(operands.اندازه() == 2);
	لگاریتم("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", سطوح, fa_شمار, c42_شمار, final_عمق);
	أرجع {operands[0].sig, operands[1].sig};
}

فراغ emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	صحيح عرض = Get_اندازه(y);
	log_assert(Get_اندازه(a) == عرض);
	log_assert(Get_اندازه(b) == عرض);

	إذا (عرض == 0)
		أرجع;

	إذا (عرض == 1) {
		module->جمع_کن_Xor_Gate(NEW_ID, a[0], b[0], y[0]);
		أرجع;
	}

	// Bit level gen and prop
	std::وکتور<Sig_بیت> g_pre(عرض), p_pre(عرض);
	لكل (صحيح i = 0; i < عرض; i++) {
		Sig_بیت gi = module->جمع_کن_Wire(NEW_ID);
		Sig_بیت pi = module->جمع_کن_Wire(NEW_ID);
		module->جمع_کن_And_Gate(NEW_ID, a[i], b[i], gi);
		module->جمع_کن_Xor_Gate(NEW_ID, a[i], b[i], pi);
		g_pre[i] = gi;
		p_pre[i] = pi;
	}

	// Propagate (g, p) through ceil(log2 W) levels
	std::وکتور<Sig_بیت> g = g_pre;
	std::وکتور<Sig_بیت> p = p_pre;
	صحيح رقم_سطوح = 0;

	طالما ((1 << رقم_سطوح) < عرض)
		رقم_سطوح++;

	لكل (صحيح k = 1; k <= رقم_سطوح; k++) {
		صحيح s = 1 << (k - 1);
		std::وکتور<Sig_بیت> g_بعدی(عرض), p_بعدی(عرض);
		لكل (صحيح i = 0; i < عرض; i++) {
			إذا (i < s) {
				// Nothing to do
				g_بعدی[i] = g[i];
				p_بعدی[i] = p[i];
			} وإلا {
				// g_i^k = g_i | (p_i & g_(i-s))
				Sig_بیت and_pg = module->جمع_کن_Wire(NEW_ID);
				module->جمع_کن_And_Gate(NEW_ID, p[i], g[i - s], and_pg);
				Sig_بیت gnew = module->جمع_کن_Wire(NEW_ID);
				module->جمع_کن_Or_Gate(NEW_ID, g[i], and_pg, gnew);
				g_بعدی[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				إذا (k < رقم_سطوح) {
					Sig_بیت pnew = module->جمع_کن_Wire(NEW_ID);
					module->جمع_کن_And_Gate(NEW_ID, p[i], p[i - s], pnew);
					p_بعدی[i] = pnew;
				} وإلا {
					// Skip last level
					p_بعدی[i] = State::Sx;
				}
			}
		}

		g = std::move(g_بعدی);
		p = std::move(p_بعدی);
	}

	// Sum layer, g[i] is COUT of bit i
	// With CIN 0:
	//   sum[0] = p_pre[0]
	//   sum[i] = p_pre[i] ^ g[i-1] ...
	module->connect(y[0], p_pre[0]);
	لكل (صحيح i = 1; i < عرض; i++)
		module->جمع_کن_Xor_Gate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	بدل (choice) {
		حالة FinalAdder::DEFAULT:
		حالة FinalAdder::RIPPLE: {
			أرجع module->جمع_کن_جمع_کن(NEW_ID, a, b, y, خطأ);
		}
		حالة FinalAdder::PARALLEL_پیشوند: {
			emit_kogge_stone(module, a, b, y);
			أرجع اشاره_تهی;
		}
	}
	log_assert(خطأ && "CompressorTree::emit_final_adder: invalid choice");
	أرجع اشاره_تهی;
}

FinalAdder pick_final_adder(صحيح عرض, صحيح final_عمق, Final_نما نما) {
	بدل (نما) {
		حالة Final_نما::RIPPLE:  أرجع FinalAdder::RIPPLE;
		حالة Final_نما::پیشوند:  أرجع FinalAdder::PARALLEL_پیشوند;
		حالة Final_نما::AUTO:
		افتراضي: {
			منطقي wide = عرض >= RIPPLE_پیشوند_عرض_آستانه;
			منطقي deep = final_عمق >= RIPPLE_پیشوند_عمق_آستانه;
			أرجع (wide && deep) ? FinalAdder::PARALLEL_پیشوند : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_انتها
