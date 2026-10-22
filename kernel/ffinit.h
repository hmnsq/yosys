/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2020  Marcelina Kościelnicka <mwk@0x04.net>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#جے_بنا_تعریف FFINIT_H
#تعریف_کرو FFINIT_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

بنية FfInitVals
{
	ثابت SigMapView *sigmap;
	dict<Sig_ثنائی_ہندسہ, std::pair<State,Sig_ثنائی_ہندسہ>> initbits;

	فراغ سیٹ(ثابت SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		لكل (خودکار wire : module->wires())
			إذا (wire->گن.گنتی(ID::init))
				process_wire(wire);
	}

	فراغ process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->گن.at(ID::init);

		لكل (صحيح i = 0; i < Get_ناپ(wirebits) && i < Get_ناپ(initval); i++)
		{
			Sig_ثنائی_ہندسہ ثنائی_ہندسہ = wirebits[i];
			State val = initval[i];

			إذا (val != State::S0 && val != State::S1 && ثنائی_ہندسہ.wire != nullptr)
				استمر;

			إذا (initbits.گنتی(ثنائی_ہندسہ)) {
				إذا (initbits.at(ثنائی_ہندسہ).پہلا != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(ثنائی_ہندسہ), log_signal(Sig_ثنائی_ہندسہ(wire, i)),
							log_signal(val), log_signal(initbits.at(ثنائی_ہندسہ).پہلا));
				استمر;
			}

			initbits[ثنائی_ہندسہ] = std::make_pair(val,Sig_ثنائی_ہندسہ(wire,i));
		}
	}

	فراغ سیٹ_parallel(ثابت SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ثابت RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_ناپ(0, module->wires_ناپ(), 1000));
		Sharded_سمتیہ<RTLIL::Wire*> init_wires(subpool);
		subpool.لگاتار([const_module, &init_wires](ثابت ParallelDispatchThreadPool::لگاتار_Ctx &ctx) {
			لكل (صحيح i : ctx.شے_range(const_module->wires_ناپ())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				إذا (wire->گن.گنتی(ID::init))
					init_wires.داخل_کرو(ctx, wire);
			}
		});

		لكل (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State عامل()(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		خودکار it = initbits.لبھو((*sigmap)(ثنائی_ہندسہ));
		إذا (it != initbits.سرا())
			أرجع it->دوجا.پہلا;
		وإلا
			أرجع State::Sx;
	}

	RTLIL::Const عامل()(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::Const::Builder res_بٹ(Get_ناپ(sig));
		لكل (خودکار ثنائی_ہندسہ : sig)
			res_بٹ.ڈالو_پٹھ((*this)(ثنائی_ہندسہ));
		أرجع res_بٹ.build();
	}

	فراغ سیٹ_init(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ, RTLIL::State val)
	{
		Sig_ثنائی_ہندسہ mbit = (*sigmap)(ثنائی_ہندسہ);
		Sig_ثنائی_ہندسہ abit = ثنائی_ہندسہ;
		خودکار it = initbits.لبھو(mbit);
		إذا (it != initbits.سرا())
			abit = it->دوجا.دوجا;
		وإلا إذا (val == State::Sx)
			أرجع;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		خودکار it2 = abit.wire->گن.لبھو(ID::init);
		إذا (it2 != abit.wire->گن.سرا()) {
			it2->دوجا.سیٹ(abit.وتھ, val);
			إذا (it2->دوجا.is_fully_undef())
				abit.wire->گن.erase(it2);
		} وإلا إذا (val != State::Sx) {
			Const cval(State::Sx, Get_ناپ(abit.wire));
			cval.سیٹ(abit.وتھ, val);
			abit.wire->گن[ID::init] = cval;
		}
	}

	فراغ سیٹ_init(ثابت RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(Get_ناپ(sig) == Get_ناپ(val));
		لكل (صحيح i = 0; i < Get_ناپ(sig); i++)
			سیٹ_init(sig[i], val[i]);
	}

	فراغ remove_init(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
	{
		سیٹ_init(ثنائی_ہندسہ, State::Sx);
	}

	فراغ remove_init(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودکار ثنائی_ہندسہ : sig)
			remove_init(ثنائی_ہندسہ);
	}

	// Move $barrier output init value to it's input
	فراغ move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> اگلا;
		لكل (خودکار cell : module->cells()) {
			إذا (cell->type != ID($barrier))
				استمر;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->get_پورٹ(ID::Y)), sig_a = cell->get_پورٹ(ID::A);
			لكل (صحيح i = 0; i < Get_ناپ(sig_y); i++)
				اگلا[sig_y[i]] = sig_a[i];
		}
		إذا (اگلا.خالی_تھاواں() || initbits.خالی_تھاواں())
			أرجع;
		pool<RTLIL::Sig_ثنائی_ہندسہ> inputs = داخلہ_nets(module), moved;
		لكل (خودکار [from, منزل] : اگلا) {
			RTLIL::State val = (*this)(from);
			إذا (val == State::Sx)
				استمر;
			moved.داخل_کرو(from);
			لكل (صحيح i = 0; i < Get_ناپ(اگلا) && اگلا.گنتی((*sigmap)(منزل)); i++)
				منزل = اگلا.at((*sigmap)(منزل));
			RTLIL::Sig_ثنائی_ہندسہ منزل_net = (*sigmap)(منزل);
			إذا (منزل_net.wire == nullptr || inputs.گنتی(منزل_net))
				استمر;
			RTLIL::State منزل_val = (*this)(منزل);
			إذا (منزل_val == State::Sx)
				سیٹ_init(منزل, val);
			وإلا إذا (منزل_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(منزل), log_signal(from), log_signal(val), log_signal(منزل_val));
		}
		إذا (!moved.خالی_تھاواں())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::Sig_ثنائی_ہندسہ> داخلہ_nets(RTLIL::Module *module) ثابت
	{
		pool<RTLIL::Sig_ثنائی_ہندسہ> nets;
		لكل (خودکار wire : module->wires())
			إذا (wire->پورٹ_داخلہ)
				لكل (خودکار ثنائی_ہندسہ : (*sigmap)(wire))
					nets.داخل_کرو(ثنائی_ہندسہ);
		أرجع nets;
	}

	فراغ remove_net_inits(RTLIL::Module *module, ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &nets)
	{
		لكل (خودکار ثنائی_ہندسہ : nets)
			initbits.erase(ثنائی_ہندسہ);
		لكل (خودکار wire : module->wires()) {
			خودکار it = wire->گن.لبھو(ID::init);
			إذا (it == wire->گن.سرا())
				استمر;
			لكل (صحيح i = 0; i < Get_ناپ(wire) && i < Get_ناپ(it->دوجا); i++)
				إذا (nets.گنتی((*sigmap)(RTLIL::Sig_ثنائی_ہندسہ(wire, i))))
					it->دوجا.سیٹ(i, State::Sx);
			إذا (it->دوجا.is_fully_undef())
				wire->گن.erase(it);
		}
	}

	فراغ clear()
	{
		initbits.clear();
	}

	FfInitVals (ثابت SigMapView *sigmap, RTLIL::Module *module)
	{
		سیٹ(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_سرا

#جے_انت
