#ضمن "compressor_tree.h"

YOSYS_NAMESPACE_BEGIN

نطاق Compressor_ونه
{

std::pair<SigSpec, SigSpec> emit_compressor_32(Module *module, SigSpec a, SigSpec b, SigSpec c, صحيح پلنوالی)
{
	SigSpec مجموعه = module->جمع_کړه_Wire(NEW_ID, پلنوالی);
	SigSpec وتنه = module->جمع_کړه_Wire(NEW_ID, پلنوالی);
	module->جمع_کړه_Fa(NEW_ID, a, b, c, وتنه, مجموعه);

	SigSpec لېږد;
	لېږد.append(State::S0);
	لېږد.append(وتنه.ایستل(0, پلنوالی - 1));
	أرجع {مجموعه, لېږد};
}

std::pair<SigSpec, SigSpec> emit_compressor_42(Module *module, SigSpec a, SigSpec b, SigSpec c, SigSpec d, صحيح پلنوالی)
{
	// First FA: a + b + c -> s0
	SigSpec s0 = module->جمع_کړه_Wire(NEW_ID, پلنوالی);
	SigSpec cout_h_ډک = module->جمع_کړه_Wire(NEW_ID, پلنوالی);
	module->جمع_کړه_Fa(NEW_ID, a, b, c, cout_h_ډک, s0);

	// cin[0] = 0, cin[i] = cout_h_full[i-1]
	SigSpec ننوتنه;
	ننوتنه.append(State::S0);
	إذا (پلنوالی > 1)
		ننوتنه.append(cout_h_ډک.ایستل(0, پلنوالی - 1));

	// Second FA: s0 + d + cin -> sum
	SigSpec مجموعه = module->جمع_کړه_Wire(NEW_ID, پلنوالی);
	SigSpec لېږد_ډک = module->جمع_کړه_Wire(NEW_ID, پلنوالی);
	module->جمع_کړه_Fa(NEW_ID, s0, d, ننوتنه, لېږد_ډک, مجموعه);

	SigSpec لېږد;
	لېږد.append(State::S0);
	إذا (پلنوالی > 1)
		لېږد.append(لېږد_ډک.ایستل(0, پلنوالی - 1));

	أرجع {مجموعه, لېږد};
}

std::ویکټور<ژوروالی_Sig> generate_partial_products(Module *module, SigSpec a, SigSpec b, منطقي a_signed, منطقي b_signed, صحيح پلنوالی) {
	صحيح پلنوالی_a = Get_اندازه(a);
	صحيح پلنوالی_b = Get_اندازه(b);
	std::ویکټور<ژوروالی_Sig> products;
	products.reserve(پلنوالی_a + 3);

	لكل (صحيح i = 0; i < پلنوالی_a; i++) {
		Sig_بټ ai = a[i];

		// b_shifted = (0_i ## b)
		SigSpec b_shifted = SigSpec(State::S0, i);
		b_shifted.append(b);
		b_shifted.extend_u0(پلنوالی, خطأ);

		// row = b_shifted & replicate(a[i], width)
		SigSpec ai_rep = SigSpec(ai, پلنوالی);
		SigSpec لیکه = module->جمع_کړه_Wire(NEW_ID, پلنوالی);
		module->جمع_کړه_And(NEW_ID, b_shifted, ai_rep, لیکه);

		// Apply Modified Baugh-Wooley inversions for this row
		منطقي لیکه_is_bottom = (i == پلنوالی_a - 1);
		منطقي any_inversion = (لیکه_is_bottom && b_signed) || a_signed;

		إذا (any_inversion) {
			std::ویکټور<RTLIL::State> ماسک(پلنوالی, RTLIL::State::S0);

			لكل (صحيح j = 0; j < پلنوالی_b; j++) {
				صحيح ستون = i + j;
				إذا (ستون < 0 || ستون >= پلنوالی)
					استمر;
				منطقي ستون_is_ښی = (j == پلنوالی_b - 1);
				// Flip masks
				منطقي invert = (لیکه_is_bottom && b_signed) ^ (ستون_is_ښی && a_signed);
				إذا (invert)
					ماسک[ستون] = RTLIL::State::S1;
			}

			// Skip the xor entirely if the mask is all zeroes
			منطقي nonzero = خطأ;
			لكل (خپلکاری s : ماسک)
				إذا (s == RTLIL::State::S1) {
					nonzero = ریښتیا;
					اكسر;
				}
			إذا (nonzero) {
				SigSpec inverted = module->جمع_کړه_Wire(NEW_ID, پلنوالی);
				module->جمع_کړه_Xor(NEW_ID, لیکه, SigSpec(RTLIL::Const(ماسک)), inverted);
				لیکه = inverted;
			}
		}

		products.ټېل_شا({لیکه, 0});
	}

	// Correction constants
	خپلکاری ټېل_one_at = [&](صحيح ستون) {
		إذا (ستون < 0 || ستون >= پلنوالی)
			أرجع;
		std::ویکټور<RTLIL::State> v(پلنوالی, RTLIL::State::S0);
		v[ستون] = RTLIL::State::S1;
		products.ټېل_شا({SigSpec(RTLIL::Const(v)), 0});
	};

	إذا (b_signed)
		ټېل_one_at(پلنوالی_a - 1);
	إذا (a_signed)
		ټېل_one_at(پلنوالی_b - 1);
	إذا (a_signed || b_signed) {
		// Sign-extend the Baugh-Wooley correction to the accumulator width.
		صحيح sign_extension_پیل = پلنوالی_a + پلنوالی_b - 1;
		إذا (sign_extension_پیل < پلنوالی) {
			std::ویکټور<RTLIL::State> v(پلنوالی, RTLIL::State::S0);
			لكل (صحيح i = sign_extension_پیل; i < پلنوالی; i++)
				v[i] = RTLIL::State::S1;
			products.ټېل_شا({SigSpec(RTLIL::Const(v)), 0});
		}
	}

	أرجع products;
}

std::pair<SigSpec, SigSpec> reduce_scheduled(Module *module, std::ویکټور<ژوروالی_Sig> operands, صحيح پلنوالی, Strategy strategy, صحيح *بهر_compressor_شمېر, صحيح *بهر_final_ژوروالی) {
	صحيح پوړونه = 0;
	صحيح fa_شمېر = 0;
	صحيح c42_شمېر = 0;
	صحيح final_ژوروالی = 0;

	لكل (خپلکاری &op : operands)
		op.sig.extend_u0(پلنوالی);

	// Only compress operands ready at current level
	لكل (صحيح level = 0; operands.اندازه() > 2; level++) {
		// Partition operands into ready and waiting
		std::ویکټور<ژوروالی_Sig> ready;
		std::ویکټور<ژوروالی_Sig> انتظار;
		ready.reserve(operands.اندازه());
		لكل (خپلکاری &op : operands) {
			إذا (op.ژوروالی <= level)
				ready.ټېل_شا(op);
			وإلا
				انتظار.ټېل_شا(op);
		}

		إذا (ready.اندازه() < 3) {
			پوړونه++;
			استمر;
		}

		// Apply compressors to ready operands
		std::ویکټور<ژوروالی_Sig> compressed;
		compressed.reserve(ready.اندازه());
		اندازه_t i = 0;

		// PREFER_42 attempts 4:2 grouping greedily (falls back to 3:2 for the residual)
		// FA_ONLY skips
		// DADDA = PREFER_42 (TODO: inspect column heights?)
		منطقي try_42 = (strategy == Strategy::PREFER_42 || strategy == Strategy::DADDA);

		طالما (i < ready.اندازه()) {
			اندازه_t پاتې_شوی = ready.اندازه() - i;

			إذا (try_42 && پاتې_شوی >= 4) {
				ژوروالی_Sig a = ready[i + 0];
				ژوروالی_Sig b = ready[i + 1];
				ژوروالی_Sig c = ready[i + 2];
				ژوروالی_Sig d = ready[i + 3];

				خپلکاری [مجموعه, لېږد] = emit_compressor_42(module, a.sig, b.sig, c.sig, d.sig, پلنوالی);
				صحيح dmax = std::max({a.ژوروالی, b.ژوروالی, c.ژوروالی, d.ژوروالی});

				compressed.ټېل_شا({مجموعه, dmax + 2});
				compressed.ټېل_شا({لېږد, dmax + 2});

				fa_شمېر += 2;
				c42_شمېر += 1;
				i += 4;
			} وإلا إذا (پاتې_شوی >= 3) {
				ژوروالی_Sig a = ready[i + 0];
				ژوروالی_Sig b = ready[i + 1];
				ژوروالی_Sig c = ready[i + 2];

				خپلکاری [مجموعه, لېږد] = emit_compressor_32(module, a.sig, b.sig, c.sig, پلنوالی);
				صحيح dmax = std::max({a.ژوروالی, b.ژوروالی, c.ژوروالی});

				compressed.ټېل_شا({مجموعه, dmax + 1});
				compressed.ټېل_شا({لېږد, dmax + 1});

				fa_شمېر += 1;
				i += 3;
			} وإلا {
				// Uncompressed operands pass through to next level
				لكل (; i < ready.اندازه(); i++)
					compressed.ټېل_شا(ready[i]);
				اكسر;
			}
		}

		// Merge compressed with waiting operands
		لكل (خپلکاری &op : انتظار)
			compressed.ټېل_شا(op);

		operands = std::move(compressed);
		پوړونه++;
	}

	إذا(بهر_compressor_شمېر)
		*بهر_compressor_شمېر = fa_شمېر;
	إذا (operands.اندازه() == 0) {
		إذا (بهر_final_ژوروالی)
			*بهر_final_ژوروالی = 0;
		أرجع {SigSpec(State::S0, پلنوالی), SigSpec(State::S0, پلنوالی)};
	}
	إذا (operands.اندازه() == 1) {
		إذا (بهر_final_ژوروالی)
			*بهر_final_ژوروالی = operands[0].ژوروالی;
		أرجع {operands[0].sig, SigSpec(State::S0, پلنوالی)};
	}

	final_ژوروالی = std::max(operands[0].ژوروالی, operands[1].ژوروالی);
	إذا (بهر_final_ژوروالی)
		*بهر_final_ژوروالی = final_ژوروالی;
	log_assert(operands.اندازه() == 2);
	لوګاریتم("    CompressorTree::reduce_scheduled: %d levels, %d $fa (%d as 4:2), final depth %d\n", پوړونه, fa_شمېر, c42_شمېر, final_ژوروالی);
	أرجع {operands[0].sig, operands[1].sig};
}

فراغ emit_kogge_stone(Module *module, SigSpec a, SigSpec b, SigSpec y)
{
	صحيح پلنوالی = Get_اندازه(y);
	log_assert(Get_اندازه(a) == پلنوالی);
	log_assert(Get_اندازه(b) == پلنوالی);

	إذا (پلنوالی == 0)
		أرجع;

	إذا (پلنوالی == 1) {
		module->جمع_کړه_Xor_Gate(NEW_ID, a[0], b[0], y[0]);
		أرجع;
	}

	// Bit level gen and prop
	std::ویکټور<Sig_بټ> g_pre(پلنوالی), p_pre(پلنوالی);
	لكل (صحيح i = 0; i < پلنوالی; i++) {
		Sig_بټ gi = module->جمع_کړه_Wire(NEW_ID);
		Sig_بټ pi = module->جمع_کړه_Wire(NEW_ID);
		module->جمع_کړه_And_Gate(NEW_ID, a[i], b[i], gi);
		module->جمع_کړه_Xor_Gate(NEW_ID, a[i], b[i], pi);
		g_pre[i] = gi;
		p_pre[i] = pi;
	}

	// Propagate (g, p) through ceil(log2 W) levels
	std::ویکټور<Sig_بټ> g = g_pre;
	std::ویکټور<Sig_بټ> p = p_pre;
	صحيح عدد_پوړونه = 0;

	طالما ((1 << عدد_پوړونه) < پلنوالی)
		عدد_پوړونه++;

	لكل (صحيح k = 1; k <= عدد_پوړونه; k++) {
		صحيح s = 1 << (k - 1);
		std::ویکټور<Sig_بټ> g_بل(پلنوالی), p_بل(پلنوالی);
		لكل (صحيح i = 0; i < پلنوالی; i++) {
			إذا (i < s) {
				// Nothing to do
				g_بل[i] = g[i];
				p_بل[i] = p[i];
			} وإلا {
				// g_i^k = g_i | (p_i & g_(i-s))
				Sig_بټ and_pg = module->جمع_کړه_Wire(NEW_ID);
				module->جمع_کړه_And_Gate(NEW_ID, p[i], g[i - s], and_pg);
				Sig_بټ gnew = module->جمع_کړه_Wire(NEW_ID);
				module->جمع_کړه_Or_Gate(NEW_ID, g[i], and_pg, gnew);
				g_بل[i] = gnew;

				// p_i^k = p_i & p_(i-s)
				إذا (k < عدد_پوړونه) {
					Sig_بټ pnew = module->جمع_کړه_Wire(NEW_ID);
					module->جمع_کړه_And_Gate(NEW_ID, p[i], p[i - s], pnew);
					p_بل[i] = pnew;
				} وإلا {
					// Skip last level
					p_بل[i] = State::Sx;
				}
			}
		}

		g = std::move(g_بل);
		p = std::move(p_بل);
	}

	// Sum layer, g[i] is COUT of bit i
	// With CIN 0:
	//   sum[0] = p_pre[0]
	//   sum[i] = p_pre[i] ^ g[i-1] ...
	module->connect(y[0], p_pre[0]);
	لكل (صحيح i = 1; i < پلنوالی; i++)
		module->جمع_کړه_Xor_Gate(NEW_ID, p_pre[i], g[i - 1], y[i]);
}

Cell *emit_final_adder(Module *module, SigSpec a, SigSpec b, SigSpec y, FinalAdder choice) {
	بدل (choice) {
		حالة FinalAdder::DEFAULT:
		حالة FinalAdder::RIPPLE: {
			أرجع module->جمع_کړه_جمع_کړه(NEW_ID, a, b, y, خطأ);
		}
		حالة FinalAdder::PARALLEL_مختاړی: {
			emit_kogge_stone(module, a, b, y);
			أرجع تش_اشاره;
		}
	}
	log_assert(خطأ && "CompressorTree::emit_final_adder: invalid choice");
	أرجع تش_اشاره;
}

FinalAdder pick_final_adder(صحيح پلنوالی, صحيح final_ژوروالی, Final_ډېرپېښ ډېرپېښ) {
	بدل (ډېرپېښ) {
		حالة Final_ډېرپېښ::RIPPLE:  أرجع FinalAdder::RIPPLE;
		حالة Final_ډېرپېښ::مختاړی:  أرجع FinalAdder::PARALLEL_مختاړی;
		حالة Final_ډېرپېښ::AUTO:
		افتراضي: {
			منطقي wide = پلنوالی >= RIPPLE_مختاړی_پلنوالی_پوله;
			منطقي deep = final_ژوروالی >= RIPPLE_مختاړی_ژوروالی_پوله;
			أرجع (wide && deep) ? FinalAdder::PARALLEL_مختاړی : FinalAdder::DEFAULT;
		}
	}
}

} // namespace CompressorTree

YOSYS_NAMESPACE_پای_ټکی
