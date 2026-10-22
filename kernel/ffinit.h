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

#اگر_غیر_تعریف_شدہ FFINIT_H
#تعریف_کرو FFINIT_H

#شامل "kernel/yosys.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ساخت FfInitVals
{
	ثابت SigMapView *sigmap;
	dict<Sig_ثنائی_ہندسہ, std::pair<State,Sig_ثنائی_ہندسہ>> initbits;

	خالی سیٹ(ثابت SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		برائے (خودکار wire : module->wires())
			اگر (wire->اوصاف.گنتی(ID::init))
				process_wire(wire);
	}

	خالی process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->اوصاف.at(ID::init);

		برائے (صحیح i = 0; i < Get_سائز(wirebits) && i < Get_سائز(initval); i++)
		{
			Sig_ثنائی_ہندسہ ثنائی_ہندسہ = wirebits[i];
			State val = initval[i];

			اگر (val != State::S0 && val != State::S1 && ثنائی_ہندسہ.wire != nullptr)
				جاری;

			اگر (initbits.گنتی(ثنائی_ہندسہ)) {
				اگر (initbits.at(ثنائی_ہندسہ).پہلا != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(ثنائی_ہندسہ), log_signal(Sig_ثنائی_ہندسہ(wire, i)),
							log_signal(val), log_signal(initbits.at(ثنائی_ہندسہ).پہلا));
				جاری;
			}

			initbits[ثنائی_ہندسہ] = std::make_pair(val,Sig_ثنائی_ہندسہ(wire,i));
		}
	}

	خالی سیٹ_parallel(ثابت SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ثابت RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_سائز(0, module->wires_سائز(), 1000));
		Sharded_سمتیہ<RTLIL::Wire*> init_wires(subpool);
		subpool.تسلسل([const_module, &init_wires](ثابت ParallelDispatchThreadPool::تسلسل_Ctx &ctx) {
			برائے (صحیح i : ctx.شے_range(const_module->wires_سائز())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				اگر (wire->اوصاف.گنتی(ID::init))
					init_wires.داخل_کرو(ctx, wire);
			}
		});

		برائے (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State عامل()(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		خودکار it = initbits.ڈھونڈو((*sigmap)(ثنائی_ہندسہ));
		اگر (it != initbits.انتہا())
			واپس it->دوسرا.پہلا;
		ورنہ
			واپس State::Sx;
	}

	RTLIL::Const عامل()(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::Const::Builder res_بٹ(Get_سائز(sig));
		برائے (خودکار ثنائی_ہندسہ : sig)
			res_بٹ.ڈالو_عقب((*this)(ثنائی_ہندسہ));
		واپس res_بٹ.build();
	}

	خالی سیٹ_init(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ, RTLIL::State val)
	{
		Sig_ثنائی_ہندسہ mbit = (*sigmap)(ثنائی_ہندسہ);
		Sig_ثنائی_ہندسہ abit = ثنائی_ہندسہ;
		خودکار it = initbits.ڈھونڈو(mbit);
		اگر (it != initbits.انتہا())
			abit = it->دوسرا.دوسرا;
		ورنہ اگر (val == State::Sx)
			واپس;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		خودکار it2 = abit.wire->اوصاف.ڈھونڈو(ID::init);
		اگر (it2 != abit.wire->اوصاف.انتہا()) {
			it2->دوسرا.سیٹ(abit.انحراف, val);
			اگر (it2->دوسرا.is_fully_undef())
				abit.wire->اوصاف.erase(it2);
		} ورنہ اگر (val != State::Sx) {
			Const cval(State::Sx, Get_سائز(abit.wire));
			cval.سیٹ(abit.انحراف, val);
			abit.wire->اوصاف[ID::init] = cval;
		}
	}

	خالی سیٹ_init(ثابت RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(Get_سائز(sig) == Get_سائز(val));
		برائے (صحیح i = 0; i < Get_سائز(sig); i++)
			سیٹ_init(sig[i], val[i]);
	}

	خالی remove_init(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
	{
		سیٹ_init(ثنائی_ہندسہ, State::Sx);
	}

	خالی remove_init(ثابت RTLIL::SigSpec &sig)
	{
		برائے (خودکار ثنائی_ہندسہ : sig)
			remove_init(ثنائی_ہندسہ);
	}

	// Move $barrier output init value to it's input
	خالی move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::Sig_ثنائی_ہندسہ, RTLIL::Sig_ثنائی_ہندسہ> اگلا;
		برائے (خودکار cell : module->cells()) {
			اگر (cell->type != ID($barrier))
				جاری;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->get_پورٹ(ID::Y)), sig_a = cell->get_پورٹ(ID::A);
			برائے (صحیح i = 0; i < Get_سائز(sig_y); i++)
				اگلا[sig_y[i]] = sig_a[i];
		}
		اگر (اگلا.خالی_خانے() || initbits.خالی_خانے())
			واپس;
		pool<RTLIL::Sig_ثنائی_ہندسہ> inputs = داخلہ_nets(module), moved;
		برائے (خودکار [from, مقصد] : اگلا) {
			RTLIL::State val = (*this)(from);
			اگر (val == State::Sx)
				جاری;
			moved.داخل_کرو(from);
			برائے (صحیح i = 0; i < Get_سائز(اگلا) && اگلا.گنتی((*sigmap)(مقصد)); i++)
				مقصد = اگلا.at((*sigmap)(مقصد));
			RTLIL::Sig_ثنائی_ہندسہ مقصد_net = (*sigmap)(مقصد);
			اگر (مقصد_net.wire == nullptr || inputs.گنتی(مقصد_net))
				جاری;
			RTLIL::State مقصد_val = (*this)(مقصد);
			اگر (مقصد_val == State::Sx)
				سیٹ_init(مقصد, val);
			ورنہ اگر (مقصد_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(مقصد), log_signal(from), log_signal(val), log_signal(مقصد_val));
		}
		اگر (!moved.خالی_خانے())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::Sig_ثنائی_ہندسہ> داخلہ_nets(RTLIL::Module *module) ثابت
	{
		pool<RTLIL::Sig_ثنائی_ہندسہ> nets;
		برائے (خودکار wire : module->wires())
			اگر (wire->پورٹ_داخلہ)
				برائے (خودکار ثنائی_ہندسہ : (*sigmap)(wire))
					nets.داخل_کرو(ثنائی_ہندسہ);
		واپس nets;
	}

	خالی remove_net_inits(RTLIL::Module *module, ثابت pool<RTLIL::Sig_ثنائی_ہندسہ> &nets)
	{
		برائے (خودکار ثنائی_ہندسہ : nets)
			initbits.erase(ثنائی_ہندسہ);
		برائے (خودکار wire : module->wires()) {
			خودکار it = wire->اوصاف.ڈھونڈو(ID::init);
			اگر (it == wire->اوصاف.انتہا())
				جاری;
			برائے (صحیح i = 0; i < Get_سائز(wire) && i < Get_سائز(it->دوسرا); i++)
				اگر (nets.گنتی((*sigmap)(RTLIL::Sig_ثنائی_ہندسہ(wire, i))))
					it->دوسرا.سیٹ(i, State::Sx);
			اگر (it->دوسرا.is_fully_undef())
				wire->اوصاف.erase(it);
		}
	}

	خالی clear()
	{
		initbits.clear();
	}

	FfInitVals (ثابت SigMapView *sigmap, RTLIL::Module *module)
	{
		سیٹ(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_انتہا

#اگر_ختم
