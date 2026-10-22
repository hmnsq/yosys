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

#اگر_تعریف_نشده FFINIT_H
#تعریف_کن FFINIT_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

بنية FfInitVals
{
	ثابت SigMapView *sigmap;
	dict<Sig_بیت, std::pair<State,Sig_بیت>> initbits;

	فراغ مجموعه(ثابت SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		لكل (خودکار wire : module->wires())
			إذا (wire->صفات.شمار(ID::init))
				process_wire(wire);
	}

	فراغ process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->صفات.at(ID::init);

		لكل (صحيح i = 0; i < Get_اندازه(wirebits) && i < Get_اندازه(initval); i++)
		{
			Sig_بیت بیت = wirebits[i];
			State val = initval[i];

			إذا (val != State::S0 && val != State::S1 && بیت.wire != nullptr)
				استمر;

			إذا (initbits.شمار(بیت)) {
				إذا (initbits.at(بیت).اولی != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(بیت), log_signal(Sig_بیت(wire, i)),
							log_signal(val), log_signal(initbits.at(بیت).اولی));
				استمر;
			}

			initbits[بیت] = std::make_pair(val,Sig_بیت(wire,i));
		}
	}

	فراغ مجموعه_parallel(ثابت SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ثابت RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_اندازه(0, module->wires_اندازه(), 1000));
		Sharded_وکتور<RTLIL::Wire*> init_wires(subpool);
		subpool.پیاپی([const_module, &init_wires](ثابت ParallelDispatchThreadPool::پیاپی_Ctx &ctx) {
			لكل (صحيح i : ctx.قلم_range(const_module->wires_اندازه())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				إذا (wire->صفات.شمار(ID::init))
					init_wires.درج(ctx, wire);
			}
		});

		لكل (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State عملگر()(RTLIL::Sig_بیت بیت) ثابت
	{
		خودکار it = initbits.پیدا_کن((*sigmap)(بیت));
		إذا (it != initbits.انتها())
			أرجع it->دومی.اولی;
		وإلا
			أرجع State::Sx;
	}

	RTLIL::Const عملگر()(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::Const::Builder res_بیتها(Get_اندازه(sig));
		لكل (خودکار بیت : sig)
			res_بیتها.افزودن_پشت((*this)(بیت));
		أرجع res_بیتها.build();
	}

	فراغ مجموعه_init(RTLIL::Sig_بیت بیت, RTLIL::State val)
	{
		Sig_بیت mbit = (*sigmap)(بیت);
		Sig_بیت abit = بیت;
		خودکار it = initbits.پیدا_کن(mbit);
		إذا (it != initbits.انتها())
			abit = it->دومی.دومی;
		وإلا إذا (val == State::Sx)
			أرجع;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		خودکار it2 = abit.wire->صفات.پیدا_کن(ID::init);
		إذا (it2 != abit.wire->صفات.انتها()) {
			it2->دومی.مجموعه(abit.جابه_جایی, val);
			إذا (it2->دومی.is_fully_undef())
				abit.wire->صفات.erase(it2);
		} وإلا إذا (val != State::Sx) {
			Const cval(State::Sx, Get_اندازه(abit.wire));
			cval.مجموعه(abit.جابه_جایی, val);
			abit.wire->صفات[ID::init] = cval;
		}
	}

	فراغ مجموعه_init(ثابت RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(Get_اندازه(sig) == Get_اندازه(val));
		لكل (صحيح i = 0; i < Get_اندازه(sig); i++)
			مجموعه_init(sig[i], val[i]);
	}

	فراغ remove_init(RTLIL::Sig_بیت بیت)
	{
		مجموعه_init(بیت, State::Sx);
	}

	فراغ remove_init(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودکار بیت : sig)
			remove_init(بیت);
	}

	// Move $barrier output init value to it's input
	فراغ move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::Sig_بیت, RTLIL::Sig_بیت> بعدی;
		لكل (خودکار cell : module->cells()) {
			إذا (cell->type != ID($barrier))
				استمر;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->get_پورت(ID::Y)), sig_a = cell->get_پورت(ID::A);
			لكل (صحيح i = 0; i < Get_اندازه(sig_y); i++)
				بعدی[sig_y[i]] = sig_a[i];
		}
		إذا (بعدی.خالی() || initbits.خالی())
			أرجع;
		pool<RTLIL::Sig_بیت> inputs = درون_داد_nets(module), moved;
		لكل (خودکار [from, مقصد] : بعدی) {
			RTLIL::State val = (*this)(from);
			إذا (val == State::Sx)
				استمر;
			moved.درج(from);
			لكل (صحيح i = 0; i < Get_اندازه(بعدی) && بعدی.شمار((*sigmap)(مقصد)); i++)
				مقصد = بعدی.at((*sigmap)(مقصد));
			RTLIL::Sig_بیت مقصد_net = (*sigmap)(مقصد);
			إذا (مقصد_net.wire == nullptr || inputs.شمار(مقصد_net))
				استمر;
			RTLIL::State مقصد_val = (*this)(مقصد);
			إذا (مقصد_val == State::Sx)
				مجموعه_init(مقصد, val);
			وإلا إذا (مقصد_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(مقصد), log_signal(from), log_signal(val), log_signal(مقصد_val));
		}
		إذا (!moved.خالی())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::Sig_بیت> درون_داد_nets(RTLIL::Module *module) ثابت
	{
		pool<RTLIL::Sig_بیت> nets;
		لكل (خودکار wire : module->wires())
			إذا (wire->پورت_درون_داد)
				لكل (خودکار بیت : (*sigmap)(wire))
					nets.درج(بیت);
		أرجع nets;
	}

	فراغ remove_net_inits(RTLIL::Module *module, ثابت pool<RTLIL::Sig_بیت> &nets)
	{
		لكل (خودکار بیت : nets)
			initbits.erase(بیت);
		لكل (خودکار wire : module->wires()) {
			خودکار it = wire->صفات.پیدا_کن(ID::init);
			إذا (it == wire->صفات.انتها())
				استمر;
			لكل (صحيح i = 0; i < Get_اندازه(wire) && i < Get_اندازه(it->دومی); i++)
				إذا (nets.شمار((*sigmap)(RTLIL::Sig_بیت(wire, i))))
					it->دومی.مجموعه(i, State::Sx);
			إذا (it->دومی.is_fully_undef())
				wire->صفات.erase(it);
		}
	}

	فراغ clear()
	{
		initbits.clear();
	}

	FfInitVals (ثابت SigMapView *sigmap, RTLIL::Module *module)
	{
		مجموعه(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_انتها

#پایان_اگر
