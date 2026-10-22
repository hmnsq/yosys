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

#که_نه_تعریف_شوی FFINIT_H
#تعریف_کړه FFINIT_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

بنية FfInitVals
{
	ثابت SigMapView *sigmap;
	dict<Sig_بټ, std::pair<State,Sig_بټ>> initbits;

	فراغ سېټ(ثابت SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		لكل (خپلکاری wire : module->wires())
			إذا (wire->ځانګړتیاوې.شمېر(ID::init))
				process_wire(wire);
	}

	فراغ process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->ځانګړتیاوې.at(ID::init);

		لكل (صحيح i = 0; i < Get_اندازه(wirebits) && i < Get_اندازه(initval); i++)
		{
			Sig_بټ بټ = wirebits[i];
			State val = initval[i];

			إذا (val != State::S0 && val != State::S1 && بټ.wire != nullptr)
				استمر;

			إذا (initbits.شمېر(بټ)) {
				إذا (initbits.at(بټ).لومړی != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(بټ), log_signal(Sig_بټ(wire, i)),
							log_signal(val), log_signal(initbits.at(بټ).لومړی));
				استمر;
			}

			initbits[بټ] = std::make_pair(val,Sig_بټ(wire,i));
		}
	}

	فراغ سېټ_parallel(ثابت SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ثابت RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_اندازه(0, module->wires_اندازه(), 1000));
		Sharded_ویکټور<RTLIL::Wire*> init_wires(subpool);
		subpool.پرلپسې([const_module, &init_wires](ثابت ParallelDispatchThreadPool::پرلپسې_Ctx &ctx) {
			لكل (صحيح i : ctx.توکی_range(const_module->wires_اندازه())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				إذا (wire->ځانګړتیاوې.شمېر(ID::init))
					init_wires.ورننباسه(ctx, wire);
			}
		});

		لكل (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State چلوونکی()(RTLIL::Sig_بټ بټ) ثابت
	{
		خپلکاری it = initbits.پیدا_کړه((*sigmap)(بټ));
		إذا (it != initbits.پای_ټکی())
			أرجع it->دوهم.لومړی;
		وإلا
			أرجع State::Sx;
	}

	RTLIL::Const چلوونکی()(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::Const::Builder res_بټونه(Get_اندازه(sig));
		لكل (خپلکاری بټ : sig)
			res_بټونه.ټېل_شا((*this)(بټ));
		أرجع res_بټونه.build();
	}

	فراغ سېټ_init(RTLIL::Sig_بټ بټ, RTLIL::State val)
	{
		Sig_بټ mbit = (*sigmap)(بټ);
		Sig_بټ abit = بټ;
		خپلکاری it = initbits.پیدا_کړه(mbit);
		إذا (it != initbits.پای_ټکی())
			abit = it->دوهم.دوهم;
		وإلا إذا (val == State::Sx)
			أرجع;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		خپلکاری it2 = abit.wire->ځانګړتیاوې.پیدا_کړه(ID::init);
		إذا (it2 != abit.wire->ځانګړتیاوې.پای_ټکی()) {
			it2->دوهم.سېټ(abit.دننه_واټن, val);
			إذا (it2->دوهم.is_fully_undef())
				abit.wire->ځانګړتیاوې.erase(it2);
		} وإلا إذا (val != State::Sx) {
			Const cval(State::Sx, Get_اندازه(abit.wire));
			cval.سېټ(abit.دننه_واټن, val);
			abit.wire->ځانګړتیاوې[ID::init] = cval;
		}
	}

	فراغ سېټ_init(ثابت RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(Get_اندازه(sig) == Get_اندازه(val));
		لكل (صحيح i = 0; i < Get_اندازه(sig); i++)
			سېټ_init(sig[i], val[i]);
	}

	فراغ remove_init(RTLIL::Sig_بټ بټ)
	{
		سېټ_init(بټ, State::Sx);
	}

	فراغ remove_init(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خپلکاری بټ : sig)
			remove_init(بټ);
	}

	// Move $barrier output init value to it's input
	فراغ move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::Sig_بټ, RTLIL::Sig_بټ> بل;
		لكل (خپلکاری cell : module->cells()) {
			إذا (cell->type != ID($barrier))
				استمر;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->get_پورټ(ID::Y)), sig_a = cell->get_پورټ(ID::A);
			لكل (صحيح i = 0; i < Get_اندازه(sig_y); i++)
				بل[sig_y[i]] = sig_a[i];
		}
		إذا (بل.تشه() || initbits.تشه())
			أرجع;
		pool<RTLIL::Sig_بټ> inputs = ننوت_nets(module), moved;
		لكل (خپلکاری [from, منزل] : بل) {
			RTLIL::State val = (*this)(from);
			إذا (val == State::Sx)
				استمر;
			moved.ورننباسه(from);
			لكل (صحيح i = 0; i < Get_اندازه(بل) && بل.شمېر((*sigmap)(منزل)); i++)
				منزل = بل.at((*sigmap)(منزل));
			RTLIL::Sig_بټ منزل_net = (*sigmap)(منزل);
			إذا (منزل_net.wire == nullptr || inputs.شمېر(منزل_net))
				استمر;
			RTLIL::State منزل_val = (*this)(منزل);
			إذا (منزل_val == State::Sx)
				سېټ_init(منزل, val);
			وإلا إذا (منزل_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(منزل), log_signal(from), log_signal(val), log_signal(منزل_val));
		}
		إذا (!moved.تشه())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::Sig_بټ> ننوت_nets(RTLIL::Module *module) ثابت
	{
		pool<RTLIL::Sig_بټ> nets;
		لكل (خپلکاری wire : module->wires())
			إذا (wire->پورټ_ننوت)
				لكل (خپلکاری بټ : (*sigmap)(wire))
					nets.ورننباسه(بټ);
		أرجع nets;
	}

	فراغ remove_net_inits(RTLIL::Module *module, ثابت pool<RTLIL::Sig_بټ> &nets)
	{
		لكل (خپلکاری بټ : nets)
			initbits.erase(بټ);
		لكل (خپلکاری wire : module->wires()) {
			خپلکاری it = wire->ځانګړتیاوې.پیدا_کړه(ID::init);
			إذا (it == wire->ځانګړتیاوې.پای_ټکی())
				استمر;
			لكل (صحيح i = 0; i < Get_اندازه(wire) && i < Get_اندازه(it->دوهم); i++)
				إذا (nets.شمېر((*sigmap)(RTLIL::Sig_بټ(wire, i))))
					it->دوهم.سېټ(i, State::Sx);
			إذا (it->دوهم.is_fully_undef())
				wire->ځانګړتیاوې.erase(it);
		}
	}

	فراغ clear()
	{
		initbits.clear();
	}

	FfInitVals (ثابت SigMapView *sigmap, RTLIL::Module *module)
	{
		سېټ(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
