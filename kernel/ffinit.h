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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FFINIT_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FFINIT_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/sigtools.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ਬਣਤਰ FfInitVals
{
	ਅਚਲ SigMapView *sigmap;
	dict<Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, std::pair<State,Sig_ਦੋਆਧਾਰੀ_ਅੰਕ>> initbits;

	ਖਾਲੀ ਸੈੱਟ(ਅਚਲ SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		ਲਈ (ਸਵੈਚਾਲਿਤ wire : module->wires())
			ਜੇ (wire->ਗੁਣ.ਗਿਣਤੀ_2(ID::init))
				process_wire(wire);
	}

	ਖਾਲੀ process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->ਗੁਣ.at(ID::init);

		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wirebits) && i < Get_ਮਾਪ(initval); i++)
		{
			Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ = wirebits[i];
			State val = initval[i];

			ਜੇ (val != State::S0 && val != State::S1 && ਦੋਆਧਾਰੀ_ਅੰਕ.wire != nullptr)
				ਜਾਰੀ;

			ਜੇ (initbits.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ)) {
				ਜੇ (initbits.at(ਦੋਆਧਾਰੀ_ਅੰਕ).ਪਹਿਲਾ != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(ਦੋਆਧਾਰੀ_ਅੰਕ), log_signal(Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(wire, i)),
							log_signal(val), log_signal(initbits.at(ਦੋਆਧਾਰੀ_ਅੰਕ).ਪਹਿਲਾ));
				ਜਾਰੀ;
			}

			initbits[ਦੋਆਧਾਰੀ_ਅੰਕ] = std::make_pair(val,Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(wire,i));
		}
	}

	ਖਾਲੀ ਸੈੱਟ_parallel(ਅਚਲ SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ਅਚਲ RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_ਮਾਪ(0, module->wires_ਮਾਪ(), 1000));
		Sharded_ਸਦਿਸ਼<RTLIL::Wire*> init_wires(subpool);
		subpool.ਲੜੀਵਾਰ([const_module, &init_wires](ਅਚਲ ParallelDispatchThreadPool::ਲੜੀਵਾਰ_Ctx &ctx) {
			ਲਈ (ਪੂਰਨ i : ctx.ਵਸਤੂ_range(const_module->wires_ਮਾਪ())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				ਜੇ (wire->ਗੁਣ.ਗਿਣਤੀ_2(ID::init))
					init_wires.ਪਾਓ(ctx, wire);
			}
		});

		ਲਈ (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State ਸੰਚਾਲਕ()(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ) ਅਚਲ
	{
		ਸਵੈਚਾਲਿਤ it = initbits.ਲੱਭੋ((*sigmap)(ਦੋਆਧਾਰੀ_ਅੰਕ));
		ਜੇ (it != initbits.ਸਿਰਾ())
			ਵਾਪਸ it->ਦੂਜਾ.ਪਹਿਲਾ;
		ਨਹੀਂਤਾਂ
			ਵਾਪਸ State::Sx;
	}

	RTLIL::Const ਸੰਚਾਲਕ()(ਅਚਲ RTLIL::SigSpec &sig) ਅਚਲ
	{
		RTLIL::Const::Builder res_ਬਿੱਟ(Get_ਮਾਪ(sig));
		ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			res_ਬਿੱਟ.ਧੱਕੋ_ਪਿੱਠ((*this)(ਦੋਆਧਾਰੀ_ਅੰਕ));
		ਵਾਪਸ res_ਬਿੱਟ.build();
	}

	ਖਾਲੀ ਸੈੱਟ_init(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::State val)
	{
		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ mbit = (*sigmap)(ਦੋਆਧਾਰੀ_ਅੰਕ);
		Sig_ਦੋਆਧਾਰੀ_ਅੰਕ abit = ਦੋਆਧਾਰੀ_ਅੰਕ;
		ਸਵੈਚਾਲਿਤ it = initbits.ਲੱਭੋ(mbit);
		ਜੇ (it != initbits.ਸਿਰਾ())
			abit = it->ਦੂਜਾ.ਦੂਜਾ;
		ਨਹੀਂਤਾਂ ਜੇ (val == State::Sx)
			ਵਾਪਸ;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		ਸਵੈਚਾਲਿਤ it2 = abit.wire->ਗੁਣ.ਲੱਭੋ(ID::init);
		ਜੇ (it2 != abit.wire->ਗੁਣ.ਸਿਰਾ()) {
			it2->ਦੂਜਾ.ਸੈੱਟ(abit.ਵਿੱਥ, val);
			ਜੇ (it2->ਦੂਜਾ.is_fully_undef())
				abit.wire->ਗੁਣ.erase(it2);
		} ਨਹੀਂਤਾਂ ਜੇ (val != State::Sx) {
			Const cval(State::Sx, Get_ਮਾਪ(abit.wire));
			cval.ਸੈੱਟ(abit.ਵਿੱਥ, val);
			abit.wire->ਗੁਣ[ID::init] = cval;
		}
	}

	ਖਾਲੀ ਸੈੱਟ_init(ਅਚਲ RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(Get_ਮਾਪ(sig) == Get_ਮਾਪ(val));
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(sig); i++)
			ਸੈੱਟ_init(sig[i], val[i]);
	}

	ਖਾਲੀ remove_init(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ)
	{
		ਸੈੱਟ_init(ਦੋਆਧਾਰੀ_ਅੰਕ, State::Sx);
	}

	ਖਾਲੀ remove_init(ਅਚਲ RTLIL::SigSpec &sig)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			remove_init(ਦੋਆਧਾਰੀ_ਅੰਕ);
	}

	// Move $barrier output init value to it's input
	ਖਾਲੀ move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਅਗਲਾ;
		ਲਈ (ਸਵੈਚਾਲਿਤ cell : module->cells()) {
			ਜੇ (cell->type != ID($barrier))
				ਜਾਰੀ;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->get_ਪੋਰਟ(ID::Y)), sig_a = cell->get_ਪੋਰਟ(ID::A);
			ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(sig_y); i++)
				ਅਗਲਾ[sig_y[i]] = sig_a[i];
		}
		ਜੇ (ਅਗਲਾ.ਖਾਲੀ_ਥਾਵਾਂ() || initbits.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ;
		pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> inputs = ਨਿਵੇਸ਼_nets(module), moved;
		ਲਈ (ਸਵੈਚਾਲਿਤ [from, ਮੰਜ਼ਿਲ] : ਅਗਲਾ) {
			RTLIL::State val = (*this)(from);
			ਜੇ (val == State::Sx)
				ਜਾਰੀ;
			moved.ਪਾਓ(from);
			ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(ਅਗਲਾ) && ਅਗਲਾ.ਗਿਣਤੀ_2((*sigmap)(ਮੰਜ਼ਿਲ)); i++)
				ਮੰਜ਼ਿਲ = ਅਗਲਾ.at((*sigmap)(ਮੰਜ਼ਿਲ));
			RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਮੰਜ਼ਿਲ_net = (*sigmap)(ਮੰਜ਼ਿਲ);
			ਜੇ (ਮੰਜ਼ਿਲ_net.wire == nullptr || inputs.ਗਿਣਤੀ_2(ਮੰਜ਼ਿਲ_net))
				ਜਾਰੀ;
			RTLIL::State ਮੰਜ਼ਿਲ_val = (*this)(ਮੰਜ਼ਿਲ);
			ਜੇ (ਮੰਜ਼ਿਲ_val == State::Sx)
				ਸੈੱਟ_init(ਮੰਜ਼ਿਲ, val);
			ਨਹੀਂਤਾਂ ਜੇ (ਮੰਜ਼ਿਲ_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(ਮੰਜ਼ਿਲ), log_signal(from), log_signal(val), log_signal(ਮੰਜ਼ਿਲ_val));
		}
		ਜੇ (!moved.ਖਾਲੀ_ਥਾਵਾਂ())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> ਨਿਵੇਸ਼_nets(RTLIL::Module *module) ਅਚਲ
	{
		pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> nets;
		ਲਈ (ਸਵੈਚਾਲਿਤ wire : module->wires())
			ਜੇ (wire->ਪੋਰਟ_ਨਿਵੇਸ਼)
				ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : (*sigmap)(wire))
					nets.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਵਾਪਸ nets;
	}

	ਖਾਲੀ remove_net_inits(RTLIL::Module *module, ਅਚਲ pool<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ> &nets)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : nets)
			initbits.erase(ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਲਈ (ਸਵੈਚਾਲਿਤ wire : module->wires()) {
			ਸਵੈਚਾਲਿਤ it = wire->ਗੁਣ.ਲੱਭੋ(ID::init);
			ਜੇ (it == wire->ਗੁਣ.ਸਿਰਾ())
				ਜਾਰੀ;
			ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(wire) && i < Get_ਮਾਪ(it->ਦੂਜਾ); i++)
				ਜੇ (nets.ਗਿਣਤੀ_2((*sigmap)(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(wire, i))))
					it->ਦੂਜਾ.ਸੈੱਟ(i, State::Sx);
			ਜੇ (it->ਦੂਜਾ.is_fully_undef())
				wire->ਗੁਣ.erase(it);
		}
	}

	ਖਾਲੀ clear()
	{
		initbits.clear();
	}

	FfInitVals (ਅਚਲ SigMapView *sigmap, RTLIL::Module *module)
	{
		ਸੈੱਟ(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
