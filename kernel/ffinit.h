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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FFINIT_H
#ವ್ಯಾಖ್ಯಾನಿಸು FFINIT_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ರಚನೆ FfInitVals
{
	ಸ್ಥಿರಾಂಕ SigMapView *sigmap;
	dict<Sig_ಬಿಟ್, std::pair<State,Sig_ಬಿಟ್>> initbits;

	ಶೂನ್ಯ ಗಣ(ಸ್ಥಿರಾಂಕ SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : module->wires())
			ಆದರೆ (wire->ಗುಣಲಕ್ಷಣಗಳು.ಎಣಿಕೆ_2(ID::init))
				process_wire(wire);
	}

	ಶೂನ್ಯ process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->ಗುಣಲಕ್ಷಣಗಳು.at(ID::init);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wirebits) && i < Get_ಗಾತ್ರ(initval); i++)
		{
			Sig_ಬಿಟ್ ಬಿಟ್ = wirebits[i];
			State val = initval[i];

			ಆದರೆ (val != State::S0 && val != State::S1 && ಬಿಟ್.wire != nullptr)
				ಮುಂದುವರಿ;

			ಆದರೆ (initbits.ಎಣಿಕೆ_2(ಬಿಟ್)) {
				ಆದರೆ (initbits.at(ಬಿಟ್).ಮೊದಲ != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(ಬಿಟ್), log_signal(Sig_ಬಿಟ್(wire, i)),
							log_signal(val), log_signal(initbits.at(ಬಿಟ್).ಮೊದಲ));
				ಮುಂದುವರಿ;
			}

			initbits[ಬಿಟ್] = std::make_pair(val,Sig_ಬಿಟ್(wire,i));
		}
	}

	ಶೂನ್ಯ ಗಣ_parallel(ಸ್ಥಿರಾಂಕ SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ಸ್ಥಿರಾಂಕ RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_ಗಾತ್ರ(0, module->wires_ಗಾತ್ರ(), 1000));
		Sharded_ಸದಿಶ<RTLIL::Wire*> init_wires(subpool);
		subpool.ಸತತ([const_module, &init_wires](ಸ್ಥಿರಾಂಕ ParallelDispatchThreadPool::ಸತತ_Ctx &ctx) {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i : ctx.ಅಂಶ_range(const_module->wires_ಗಾತ್ರ())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				ಆದರೆ (wire->ಗುಣಲಕ್ಷಣಗಳು.ಎಣಿಕೆ_2(ID::init))
					init_wires.ಸೇರಿಸು(ctx, wire);
			}
		});

		ಗಾಗಿ (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State ಕ್ರಿಯಾಕಾರಕ()(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್) ಸ್ಥಿರಾಂಕ
	{
		ಸ್ವಯಂಚಾಲಿತ it = initbits.ಕಂಡುಹಿಡಿ((*sigmap)(ಬಿಟ್));
		ಆದರೆ (it != initbits.ಕೊನೆ())
			ಹಿಂತಿರುಗಿ it->ಎರಡನೇ.ಮೊದಲ;
		ಇಲ್ಲದಿದ್ದರೆ
			ಹಿಂತಿರುಗಿ State::Sx;
	}

	RTLIL::Const ಕ್ರಿಯಾಕಾರಕ()(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig) ಸ್ಥಿರಾಂಕ
	{
		RTLIL::Const::Builder res_ಬಿಟ್ಟುಗಳು(Get_ಗಾತ್ರ(sig));
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : sig)
			res_ಬಿಟ್ಟುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ((*this)(ಬಿಟ್));
		ಹಿಂತಿರುಗಿ res_ಬಿಟ್ಟುಗಳು.build();
	}

	ಶೂನ್ಯ ಗಣ_init(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್, RTLIL::State val)
	{
		Sig_ಬಿಟ್ mbit = (*sigmap)(ಬಿಟ್);
		Sig_ಬಿಟ್ abit = ಬಿಟ್;
		ಸ್ವಯಂಚಾಲಿತ it = initbits.ಕಂಡುಹಿಡಿ(mbit);
		ಆದರೆ (it != initbits.ಕೊನೆ())
			abit = it->ಎರಡನೇ.ಎರಡನೇ;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val == State::Sx)
			ಹಿಂತಿರುಗಿ;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		ಸ್ವಯಂಚಾಲಿತ it2 = abit.wire->ಗುಣಲಕ್ಷಣಗಳು.ಕಂಡುಹಿಡಿ(ID::init);
		ಆದರೆ (it2 != abit.wire->ಗುಣಲಕ್ಷಣಗಳು.ಕೊನೆ()) {
			it2->ಎರಡನೇ.ಗಣ(abit.ಸ್ಥಾನಾಂತರ, val);
			ಆದರೆ (it2->ಎರಡನೇ.is_fully_undef())
				abit.wire->ಗುಣಲಕ್ಷಣಗಳು.erase(it2);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (val != State::Sx) {
			Const cval(State::Sx, Get_ಗಾತ್ರ(abit.wire));
			cval.ಗಣ(abit.ಸ್ಥಾನಾಂತರ, val);
			abit.wire->ಗುಣಲಕ್ಷಣಗಳು[ID::init] = cval;
		}
	}

	ಶೂನ್ಯ ಗಣ_init(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(Get_ಗಾತ್ರ(sig) == Get_ಗಾತ್ರ(val));
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(sig); i++)
			ಗಣ_init(sig[i], val[i]);
	}

	ಶೂನ್ಯ remove_init(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್)
	{
		ಗಣ_init(ಬಿಟ್, State::Sx);
	}

	ಶೂನ್ಯ remove_init(ಸ್ಥಿರಾಂಕ RTLIL::SigSpec &sig)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : sig)
			remove_init(ಬಿಟ್);
	}

	// Move $barrier output init value to it's input
	ಶೂನ್ಯ move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::Sig_ಬಿಟ್, RTLIL::Sig_ಬಿಟ್> ಮುಂದಿನ;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : module->cells()) {
			ಆದರೆ (cell->type != ID($barrier))
				ಮುಂದುವರಿ;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->get_ಪೋರ್ಟ್(ID::Y)), sig_a = cell->get_ಪೋರ್ಟ್(ID::A);
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(sig_y); i++)
				ಮುಂದಿನ[sig_y[i]] = sig_a[i];
		}
		ಆದರೆ (ಮುಂದಿನ.ಖಾಲಿಗಳು() || initbits.ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ;
		pool<RTLIL::Sig_ಬಿಟ್> inputs = ಒಳಹರಿವು_nets(module), moved;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ [from, ಗಮ್ಯ] : ಮುಂದಿನ) {
			RTLIL::State val = (*this)(from);
			ಆದರೆ (val == State::Sx)
				ಮುಂದುವರಿ;
			moved.ಸೇರಿಸು(from);
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(ಮುಂದಿನ) && ಮುಂದಿನ.ಎಣಿಕೆ_2((*sigmap)(ಗಮ್ಯ)); i++)
				ಗಮ್ಯ = ಮುಂದಿನ.at((*sigmap)(ಗಮ್ಯ));
			RTLIL::Sig_ಬಿಟ್ ಗಮ್ಯ_net = (*sigmap)(ಗಮ್ಯ);
			ಆದರೆ (ಗಮ್ಯ_net.wire == nullptr || inputs.ಎಣಿಕೆ_2(ಗಮ್ಯ_net))
				ಮುಂದುವರಿ;
			RTLIL::State ಗಮ್ಯ_val = (*this)(ಗಮ್ಯ);
			ಆದರೆ (ಗಮ್ಯ_val == State::Sx)
				ಗಣ_init(ಗಮ್ಯ, val);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಗಮ್ಯ_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(ಗಮ್ಯ), log_signal(from), log_signal(val), log_signal(ಗಮ್ಯ_val));
		}
		ಆದರೆ (!moved.ಖಾಲಿಗಳು())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::Sig_ಬಿಟ್> ಒಳಹರಿವು_nets(RTLIL::Module *module) ಸ್ಥಿರಾಂಕ
	{
		pool<RTLIL::Sig_ಬಿಟ್> nets;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : module->wires())
			ಆದರೆ (wire->ಪೋರ್ಟ್_ಒಳಹರಿವು)
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : (*sigmap)(wire))
					nets.ಸೇರಿಸು(ಬಿಟ್);
		ಹಿಂತಿರುಗಿ nets;
	}

	ಶೂನ್ಯ remove_net_inits(RTLIL::Module *module, ಸ್ಥಿರಾಂಕ pool<RTLIL::Sig_ಬಿಟ್> &nets)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : nets)
			initbits.erase(ಬಿಟ್);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : module->wires()) {
			ಸ್ವಯಂಚಾಲಿತ it = wire->ಗುಣಲಕ್ಷಣಗಳು.ಕಂಡುಹಿಡಿ(ID::init);
			ಆದರೆ (it == wire->ಗುಣಲಕ್ಷಣಗಳು.ಕೊನೆ())
				ಮುಂದುವರಿ;
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(wire) && i < Get_ಗಾತ್ರ(it->ಎರಡನೇ); i++)
				ಆದರೆ (nets.ಎಣಿಕೆ_2((*sigmap)(RTLIL::Sig_ಬಿಟ್(wire, i))))
					it->ಎರಡನೇ.ಗಣ(i, State::Sx);
			ಆದರೆ (it->ಎರಡನೇ.is_fully_undef())
				wire->ಗುಣಲಕ್ಷಣಗಳು.erase(it);
		}
	}

	ಶೂನ್ಯ clear()
	{
		initbits.clear();
	}

	FfInitVals (ಸ್ಥಿರಾಂಕ SigMapView *sigmap, RTLIL::Module *module)
	{
		ಗಣ(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
