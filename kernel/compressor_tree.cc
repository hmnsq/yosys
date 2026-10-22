#ضمن "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

نطاق Compressor_وڻ
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, صحيح ويڪر)
{
	SigSpec جوڙ = module->جوڙيو_Wire(NEW_ID, ويڪر);
	SigSpec ٻاهر = module->جوڙيو_Wire(NEW_ID, ويڪر);
	module->جوڙيو_Fa(NEW_ID, a, b, c, ٻاهر, جوڙ);

	SigSpec حاصل;
	حاصل.append(State::S0);
	حاصل.append(ٻاهر.ڪڍو_2(0, ويڪر - 1));
	أرجع {جوڙ, حاصل};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, صحيح ويڪر)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->جوڙيو_Wire(NEW_ID, ويڪر);
	SigSpec cout_h_ڀريل = module->جوڙيو_Wire(NEW_ID, ويڪر);
	module->جوڙيو_Fa(NEW_ID, a, b, c, cout_h_ڀريل, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec اندر;
	اندر.append(State::S0);
	إذا (ويڪر > 1)
		اندر.append(cout_h_ڀريل.ڪڍو_2(0, ويڪر - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec جوڙ = module->جوڙيو_Wire(NEW_ID, ويڪر);
	SigSpec حاصل_ڀريل = module->جوڙيو_Wire(NEW_ID, ويڪر);
	module->جوڙيو_Fa(NEW_ID, s0, d, اندر, حاصل_ڀريل, جوڙ);

	SigSpec حاصل;
	حاصل.append(State::S0);
	إذا (ويڪر > 1)
		حاصل.append(حاصل_ڀريل.ڪڍو_2(0, ويڪر - 1));

	أرجع {جوڙ, حاصل};
}

std::ويڪٽر<اونهائي_Sig> generate_partial_products(Module *module, SigSpec a, SigSpec b, منطقي a_signed, منطقي b_signed, صحيح ويڪر) {
	صحيح ويڪر_a = Get_سائيز(a);
	صحيح ويڪر_b = Get_سائيز(b);
	std::ويڪٽر<اونهائي_Sig> products;
	products.reserve(ويڪر_a + 3);

	لكل (صحيح i = 0; i < ويڪر_a; i++) {
		Sig_ٻٽو_انگ ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(ويڪر, خطأ);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, ويڪر);
		SigSpec صف = module->جوڙيو_Wire(NEW_ID, ويڪر);
		module->جوڙيو_And(NEW_ID, b_shifted, ai_rep, صف);

		// Apply Modified Baugh-Wooley inversions for this row
		منطقي صف_is_bottom = (i == ويڪر_a - 1);
		منطقي any_inversion = (صف_is_bottom && b_signed) || a_signed;

		إذا (any_inversion) {
			std::ويڪٽر<RTLIL::State> نقاب(ويڪر, RTLIL::State::S0);

			لكل (صحيح j = 0; j < ويڪر_b; j++) {
				صحيح کانو = i + j;
				إذا (کانو < 0 || کانو >= ويڪر)
					استمر;
				منطقي کانو_is_ساڄو = (j == ويڪر_b - 1);
				// Flip masks
				منطقي invert = (صف_is_bottom && b_signed) ^ (کانو_is_ساڄو && a_signed);
				إذا (invert)
					نقاب[کانو] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			منطقي nonzero = خطأ;
			لكل (خودڪار s : نقاب)
				إذا (s == RTLIL::State::S1) {
					nonzero = سچ;
					اكسر;
				}
			إذا (nonzero) {
				SigSpec inverted = module->جوڙيو_Wire(NEW_ID, ويڪر);
				module->جوڙيو_Xor(NEW_ID, صف, SigSpec(RTLIL::Const(نقاب)), inverted);
				صف = inverted;
			}
		}

		products.ڌڪو_پٺ({صف, 0});
	}

	// Correction constants
	خودڪار ڌڪو_one_at = [&](صحيح کانو) {
		إذا (کانو < 0 || کانو >= ويڪر)
			أرجع;
		std::ويڪٽر<RTLIL::State> v(ويڪر, RTLIL::State::S0);
		v[کانو] = RTLIL::State::S1;
		products.ڌڪو_پٺ({SigSpec(RTLIL::Const(v)), 0});
	};

	إذا (b_signed)
		ڌڪو_one_at(ويڪر_a - 1);
	إذا (a_signed)
		ڌڪو_one_at(ويڪر_b - 1);
	إذا (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		صحيح sign_extension_شروعات = ويڪر_a + ويڪر_b - 1;
		إذا (sign_extension_شروعات < ويڪر) {
			std::ويڪٽر<RTLIL::State> v(ويڪر, RTLIL::State::S0);
			لكل (صحيح i = sign_extension_شروعات; i < ويڪر; i++)
				v[i] = RTLIL::State::S1;
			products.ڌڪو_پٺ({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	أرجع products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::ويڪٽر<اونهائي_Sig> operands, صحيح ويڪر, Strategy strategy, صحيح *ٻاهريون_compressor_ڳڻپ, صحيح *ٻاهريون_final_اونهائي) {
	صحيح سطحون = 0;
	صحيح fa_ڳڻپ = 0;
	صحيح c42_ڳڻپ = 0;
	صحيح final_اونهائي = 0;

	لكل (خودڪار &op : operands)
		op.sig.extend_u0(ويڪر);

	// Only compress operands ready at current level
	لكل (صحيح level = 0; operands.سائيز() > 2; level++) {
		// Partition operands into ready and waiting
		std::ويڪٽر<اونهائي_Sig> ready;
		std::ويڪٽر<اونهائي_Sig> انتظار;
		ready.reserve(operands.سائيز());
		لكل (خودڪار &op : operands) {
			إذا (op.اونهائي <= level)
				ready.ڌڪو_پٺ(op);
			وإلا
				انتظار.ڌڪو_پٺ(op);
		}

		إذا (ready.سائيز() < 3) {
			سطحون++;
			استمر;
		}

		// Apply compressors to ready operands
		std::ويڪٽر<اونهائي_Sig> compressed;
		compressed.reserve(ready.سائيز());
		سائيز_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		منطقي try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		طالما (i < ready.سائيز()) {
			سائيز_t بچيل = ready.سائيز() - i;

			إذا (try_42 && بچيل >= 4) {
				اونهائي_Sig a = ready[i + 0];
				اونهائي_Sig b = ready[i + 1];
				اونهائي_Sig c = ready[i + 2];
				اونهائي_Sig d = ready[i + 3];

				خودڪار [جوڙ, حاصل] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, ويڪر);
				صحيح dmax = std::max({a.اونهائي, b.اونهائي, c.اونهائي, d.اونهائي});

				compressed.ڌڪو_پٺ({جوڙ, dmax + 2});
				compressed.ڌڪو_پٺ({حاصل, dmax + 2});

				fa_ڳڻپ += 2;
				c42_ڳڻپ += 1;
				i += 4;
			} وإلا إذا (بچيل >= 3) {
				اونهائي_Sig a = ready[i + 0];
				اونهائي_Sig b = ready[i + 1];
				اونهائي_Sig c = ready[i + 2];

				خودڪار [جوڙ, حاصل] = emit_compressor_32(module, a.sig, b.sig, c.sig, ويڪر);
				صحيح dmax = std::max({a.اونهائي, b.اونهائي, c.اونهائي});

				compressed.ڌڪو_پٺ({جوڙ, dmax + 1});
				compressed.ڌڪو_پٺ({حاصل, dmax + 1});

				fa_ڳڻپ += 1;
				i += 3;
			} وإلا {
				// Uncompressed operands pass through to next level
				لكل (; i < ready.سائيز(); i++)
					compressed.ڌڪو_پٺ(ready[i]);
				اكسر;
			}
		}

		// Merge compressed with waiting operands
		لكل (خودڪار &op : انتظار)
			compressed.ڌڪو_پٺ(op);

		operands = std::move(compressed);
		سطحون++;
	}

	إذا(ٻاهريون_compressor_ڳڻپ)
		*ٻاهريون_compressor_ڳڻپ = fa_ڳڻپ;
	إذا (operands.سائيز() == 0) {
		إذا (ٻاهريون_final_اونهائي)
			*ٻاهريون_final_اونهائي = 0;
		أرجع {SigSpec(State::S0, ويڪر), SigSpec(State::S0, ويڪر)};
	}
	إذا (operands.سائيز() == 1) {
		إذا (ٻاهريون_final_اونهائي)
			*ٻاهريون_final_اونهائي = operands[0].اونهائي;
		أرجع {operands[0].sig, SigSpec(State::S0, ويڪر)};
	}

	final_اونهائي = std::max(operands[0].اونهائي, operands[1].اونهائي);
	إذا (ٻاهريون_final_اونهائي)
		*ٻاهريون_final_اونهائي = final_اونهائي;
	log_assert(operands.سائيز() == 2);
	لاگرٿم("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", سطحون, fa_ڳڻپ, c42_ڳڻپ, final_اونهائي);
	أرجع {operands[0].sig, operands[1].sig};
}

فراغ emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	صحيح ويڪر = Get_سائيز(y);
	log_assert(Get_سائيز(a) == ويڪر);
	log_assert(Get_سائيز(b) == ويڪر);

	إذا (ويڪر == 0)
		أرجع;

	إذا (ويڪر == 1) {
		module->جوڙيو_Xor_Gate(NEW_ID, a[0], b[0], y[0]);
		أرجع;
	}

	// Bit level gen and prop
	std::ويڪٽر<Sig_ٻٽو_انگ> g_pre(ويڪر), p_pre(ويڪر);
	لكل (صحيح i = 0; i < ويڪر; i++) {
		Sig_ٻٽو_انگ gi = module->جوڙيو_Wire(NEW_ID);
		Sig_ٻٽو_انگ pi = module->جوڙيو_Wire(NEW_ID);
		module->جوڙيو_And_Gate(NEW_ID, a[i], b[i], gi);
		module->جوڙيو_Xor_Gate(NEW_ID, a[i], b[i], pi);
		g_pre[i] = gi;
		p_pre[i] = pi;
	}

	// Propagate (g, p) through ceil(log2 W) levels
	std::ويڪٽر<Sig_ٻٽو_انگ> g = g_pre;
	std::ويڪٽر<Sig_ٻٽو_انگ> p = p_pre;
	صحيح انگ_سطحون = 0;

	طالما ((1 << انگ_سطحون) < ويڪر)
		انگ_سطحون++;

	لكل (صحيح k = 1; k <= انگ_سطحون; k++) {
		صحيح s = 1 << (k - 1);
		std::ويڪٽر<Sig_ٻٽو_انگ> g_ايندڙ(ويڪر), p_ايندڙ(ويڪر);
		لكل (صحيح i = 0; i < ويڪر; i++) {
			إذا (i < s) {
				// Nothing to do
				g_ايندڙ[i] = g[i];
				p_ايندڙ[i] = p[i];
			} وإلا {
				// g_i^k = g_i | (p_i & g_(i-s))
				Sig_ٻٽو_انگ and_pg = module->جوڙيو_Wire(NEW_ID);
				module->جوڙيو_And_Gate(NEW_ID, p[i], g[i - s], and_pg);
				Sig_ٻٽو_انگ gnew = module->جوڙيو_Wire(NEW_ID);
				module->جوڙيو_Or_Gate(NEW_ID, g[i], and_pg, gnew);
				g_ايندڙ[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				إذا (k < انگ_سطحون) {
					Sig_ٻٽو_انگ pnew = module->جوڙيو_Wire(NEW_ID);
					module->جوڙيو_And_Gate(NEW_ID, p[i], p[i - s], pnew);
					p_ايندڙ[i] = pnew;
				} وإلا {
					// Skip last level
					p_ايندڙ[i] = State::Sx;
				}
			}
		}

		g = std::move(g_ايندڙ);
		p = std::move(p_ايندڙ);
	}

	// Sum layer, g[i] is COUT of bit i
	// With CIN 0:
	//   sum[0] = p_pre[0]
	//   sum[i] = p_pre[i] ^ g[i-1] ...
	module->connect(y[0], p_pre[0]);
	لكل (صحيح i = 1; i < ويڪر; i++)
		module->جوڙيو_Xor_Gate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	بدل (choice) {
		حالة FinalAdder::DEFAULT:
		حالة FinalAdder::RIPPLE: {
			أرجع module->جوڙيو_جوڙيو(NEW_ID, a, b, y, خطأ);
		}
		حالة FinalAdder::PARALLEL_اڳياڙي: {
			emit_kogge_stone(module, a, b, y);
			أرجع خالي_اشارو;
		}
	}
	log_assert(خطأ && "CompressorTree::emit_final_adder: invalid choice");
	أرجع خالي_اشارو;
}

FinalAdder pick_final_adder(صحيح ويڪر, صحيح final_اونهائي, Final_گهڻائي گهڻائي) {
	بدل (گهڻائي) {
		حالة Final_گهڻائي::RIPPLE:  أرجع FinalAdder::RIPPLE;
		حالة Final_گهڻائي::اڳياڙي:  أرجع FinalAdder::PARALLEL_اڳياڙي;
		حالة Final_گهڻائي::AUTO:
		افتراضي: {
			منطقي wide = ويڪر >= RIPPLE_اڳياڙي_ويڪر_حد;
			منطقي deep = final_اونهائي >= RIPPLE_اڳياڙي_اونهائي_حد;
			أرجع (wide && deep) ? FinalAdder::PARALLEL_اڳياڙي : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_پڇاڙي
