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

#جيڪڏهن_وصف_نه_ٿيل FFINIT_H
#وصف_ڪريو FFINIT_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

بنية FfInitVals
{
	ثابت SigMapView *sigmap;
	dict<Sig_ٻٽو_انگ, std::pair<State,Sig_ٻٽو_انگ>> initbits;

	فراغ سيٽ(ثابت SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		لكل (خودڪار wire : module->wires())
			إذا (wire->خاصيتون.ڳڻپ(ID::init))
				process_wire(wire);
	}

	فراغ process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->خاصيتون.at(ID::init);

		لكل (صحيح i = 0; i < Get_سائيز(wirebits) && i < Get_سائيز(initval); i++)
		{
			Sig_ٻٽو_انگ ٻٽو_انگ = wirebits[i];
			State val = initval[i];

			إذا (val != State::S0 && val != State::S1 && ٻٽو_انگ.wire != nullptr)
				استمر;

			إذا (initbits.ڳڻپ(ٻٽو_انگ)) {
				إذا (initbits.at(ٻٽو_انگ).پهريون != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(ٻٽو_انگ), log_signal(Sig_ٻٽو_انگ(wire, i)),
							log_signal(val), log_signal(initbits.at(ٻٽو_انگ).پهريون));
				استمر;
			}

			initbits[ٻٽو_انگ] = std::make_pair(val,Sig_ٻٽو_انگ(wire,i));
		}
	}

	فراغ سيٽ_parallel(ثابت SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ثابت RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_سائيز(0, module->wires_سائيز(), 1000));
		Sharded_ويڪٽر<RTLIL::Wire*> init_wires(subpool);
		subpool.لاڳيتو([const_module, &init_wires](ثابت ParallelDispatchThreadPool::لاڳيتو_Ctx &ctx) {
			لكل (صحيح i : ctx.شي_range(const_module->wires_سائيز())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				إذا (wire->خاصيتون.ڳڻپ(ID::init))
					init_wires.وجهو(ctx, wire);
			}
		});

		لكل (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State عامل()(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ) ثابت
	{
		خودڪار it = initbits.ڳوليو((*sigmap)(ٻٽو_انگ));
		إذا (it != initbits.پڇاڙي())
			أرجع it->ٻيو.پهريون;
		وإلا
			أرجع State::Sx;
	}

	RTLIL::Const عامل()(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::Const::Builder res_بٽ(Get_سائيز(sig));
		لكل (خودڪار ٻٽو_انگ : sig)
			res_بٽ.ڌڪو_پٺ((*this)(ٻٽو_انگ));
		أرجع res_بٽ.build();
	}

	فراغ سيٽ_init(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ, RTLIL::State val)
	{
		Sig_ٻٽو_انگ mbit = (*sigmap)(ٻٽو_انگ);
		Sig_ٻٽو_انگ abit = ٻٽو_انگ;
		خودڪار it = initbits.ڳوليو(mbit);
		إذا (it != initbits.پڇاڙي())
			abit = it->ٻيو.ٻيو;
		وإلا إذا (val == State::Sx)
			أرجع;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		خودڪار it2 = abit.wire->خاصيتون.ڳوليو(ID::init);
		إذا (it2 != abit.wire->خاصيتون.پڇاڙي()) {
			it2->ٻيو.سيٽ(abit.وٿي, val);
			إذا (it2->ٻيو.is_fully_undef())
				abit.wire->خاصيتون.erase(it2);
		} وإلا إذا (val != State::Sx) {
			Const cval(State::Sx, Get_سائيز(abit.wire));
			cval.سيٽ(abit.وٿي, val);
			abit.wire->خاصيتون[ID::init] = cval;
		}
	}

	فراغ سيٽ_init(ثابت RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(Get_سائيز(sig) == Get_سائيز(val));
		لكل (صحيح i = 0; i < Get_سائيز(sig); i++)
			سيٽ_init(sig[i], val[i]);
	}

	فراغ remove_init(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ)
	{
		سيٽ_init(ٻٽو_انگ, State::Sx);
	}

	فراغ remove_init(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودڪار ٻٽو_انگ : sig)
			remove_init(ٻٽو_انگ);
	}

	// Move $barrier output init value to it's input
	فراغ move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::Sig_ٻٽو_انگ, RTLIL::Sig_ٻٽو_انگ> ايندڙ;
		لكل (خودڪار cell : module->cells()) {
			إذا (cell->type != ID($barrier))
				استمر;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->get_پورٽ(ID::Y)), sig_a = cell->get_پورٽ(ID::A);
			لكل (صحيح i = 0; i < Get_سائيز(sig_y); i++)
				ايندڙ[sig_y[i]] = sig_a[i];
		}
		إذا (ايندڙ.خالي_جايون() || initbits.خالي_جايون())
			أرجع;
		pool<RTLIL::Sig_ٻٽو_انگ> inputs = داخلا_nets(module), moved;
		لكل (خودڪار [from, منزل] : ايندڙ) {
			RTLIL::State val = (*this)(from);
			إذا (val == State::Sx)
				استمر;
			moved.وجهو(from);
			لكل (صحيح i = 0; i < Get_سائيز(ايندڙ) && ايندڙ.ڳڻپ((*sigmap)(منزل)); i++)
				منزل = ايندڙ.at((*sigmap)(منزل));
			RTLIL::Sig_ٻٽو_انگ منزل_net = (*sigmap)(منزل);
			إذا (منزل_net.wire == nullptr || inputs.ڳڻپ(منزل_net))
				استمر;
			RTLIL::State منزل_val = (*this)(منزل);
			إذا (منزل_val == State::Sx)
				سيٽ_init(منزل, val);
			وإلا إذا (منزل_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(منزل), log_signal(from), log_signal(val), log_signal(منزل_val));
		}
		إذا (!moved.خالي_جايون())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::Sig_ٻٽو_انگ> داخلا_nets(RTLIL::Module *module) ثابت
	{
		pool<RTLIL::Sig_ٻٽو_انگ> nets;
		لكل (خودڪار wire : module->wires())
			إذا (wire->پورٽ_داخلا)
				لكل (خودڪار ٻٽو_انگ : (*sigmap)(wire))
					nets.وجهو(ٻٽو_انگ);
		أرجع nets;
	}

	فراغ remove_net_inits(RTLIL::Module *module, ثابت pool<RTLIL::Sig_ٻٽو_انگ> &nets)
	{
		لكل (خودڪار ٻٽو_انگ : nets)
			initbits.erase(ٻٽو_انگ);
		لكل (خودڪار wire : module->wires()) {
			خودڪار it = wire->خاصيتون.ڳوليو(ID::init);
			إذا (it == wire->خاصيتون.پڇاڙي())
				استمر;
			لكل (صحيح i = 0; i < Get_سائيز(wire) && i < Get_سائيز(it->ٻيو); i++)
				إذا (nets.ڳڻپ((*sigmap)(RTLIL::Sig_ٻٽو_انگ(wire, i))))
					it->ٻيو.سيٽ(i, State::Sx);
			إذا (it->ٻيو.is_fully_undef())
				wire->خاصيتون.erase(it);
		}
	}

	فراغ clear()
	{
		initbits.clear();
	}

	FfInitVals (ثابت SigMapView *sigmap, RTLIL::Module *module)
	{
		سيٽ(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
