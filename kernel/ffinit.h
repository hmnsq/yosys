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

#ifndef FFINIT_H
#define FFINIT_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

ಸಂರಚನಾ FfInitVals
{
	ಧ್ರುವ SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	ಶೂನ್ಯ set(ಧ್ರುವ SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		ಪ್ರತಿ (auto wire : module->wires())
			ಯದಿ (wire->attributes.ಎಣಿಕೆ(ID::init))
				process_wire(wire);
	}

	ಶೂನ್ಯ process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			ಯದಿ (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				ಅನುವರ್ತ;

			ಯದಿ (initbits.ಎಣಿಕೆ(bit)) {
				ಯದಿ (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				ಅನುವರ್ತ;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	ಶೂನ್ಯ set_parallel(ಧ್ರುವ SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ಧ್ರುವ RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->wires_size(), 1000));
		ShardedVector<RTLIL::Wire*> init_wires(subpool);
		subpool.run([const_module, &init_wires](ಧ್ರುವ ParallelDispatchThreadPool::RunCtx &ctx) {
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				ಯದಿ (wire->attributes.ಎಣಿಕೆ(ID::init))
					init_wires.ಸೇರಿಸು(ctx, wire);
			}
		});

		ಪ್ರತಿ (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State operator()(RTLIL::SigBit bit) ಧ್ರುವ
	{
		auto it = initbits.find((*sigmap)(bit));
		ಯದಿ (it != initbits.end())
			ಪ್ರತ್ಯಾಗಮ it->second.first;
		ಅನ್ಯಥಾ
			ಪ್ರತ್ಯಾಗಮ State::Sx;
	}

	RTLIL::Const operator()(ಧ್ರುವ RTLIL::SigSpec &sig) ಧ್ರುವ
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		ಪ್ರತಿ (auto bit : sig)
			res_bits.ತಳ್ಳು_back((*this)(bit));
		ಪ್ರತ್ಯಾಗಮ res_bits.build();
	}

	ಶೂನ್ಯ set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		auto it = initbits.find(mbit);
		ಯದಿ (it != initbits.end())
			abit = it->second.second;
		ಅನ್ಯಥಾ ಯದಿ (val == State::Sx)
			ಪ್ರತ್ಯಾಗಮ;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		auto it2 = abit.wire->attributes.find(ID::init);
		ಯದಿ (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			ಯದಿ (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} ಅನ್ಯಥಾ ಯದಿ (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	ಶೂನ್ಯ set_init(ಧ್ರುವ RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	ಶೂನ್ಯ remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	ಶೂನ್ಯ remove_init(ಧ್ರುವ RTLIL::SigSpec &sig)
	{
		ಪ್ರತಿ (auto bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	ಶೂನ್ಯ move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		ಪ್ರತಿ (auto cell : module->cells()) {
			ಯದಿ (cell->type != ID($barrier))
				ಅನುವರ್ತ;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		ಯದಿ (next.empty() || initbits.empty())
			ಪ್ರತ್ಯಾಗಮ;
		pool<RTLIL::SigBit> inputs = ಒಳಹರಿವು_nets(module), moved;
		ಪ್ರತಿ (auto [from, to] : next) {
			RTLIL::State val = (*this)(from);
			ಯದಿ (val == State::Sx)
				ಅನುವರ್ತ;
			moved.ಸೇರಿಸು(from);
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(next) && next.ಎಣಿಕೆ((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			ಯದಿ (to_net.wire == nullptr || inputs.ಎಣಿಕೆ(to_net))
				ಅನುವರ್ತ;
			RTLIL::State to_val = (*this)(to);
			ಯದಿ (to_val == State::Sx)
				set_init(to, val);
			ಅನ್ಯಥಾ ಯದಿ (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		ಯದಿ (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> ಒಳಹರಿವು_nets(RTLIL::Module *module) ಧ್ರುವ
	{
		pool<RTLIL::SigBit> nets;
		ಪ್ರತಿ (auto wire : module->wires())
			ಯದಿ (wire->port_ಒಳಹರಿವು)
				ಪ್ರತಿ (auto bit : (*sigmap)(wire))
					nets.ಸೇರಿಸು(bit);
		ಪ್ರತ್ಯಾಗಮ nets;
	}

	ಶೂನ್ಯ remove_net_inits(RTLIL::Module *module, ಧ್ರುವ pool<RTLIL::SigBit> &nets)
	{
		ಪ್ರತಿ (auto bit : nets)
			initbits.erase(bit);
		ಪ್ರತಿ (auto wire : module->wires()) {
			auto it = wire->attributes.find(ID::init);
			ಯದಿ (it == wire->attributes.end())
				ಅನುವರ್ತ;
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				ಯದಿ (nets.ಎಣಿಕೆ((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			ಯದಿ (it->second.is_fully_undef())
				wire->attributes.erase(it);
		}
	}

	ಶೂನ್ಯ clear()
	{
		initbits.clear();
	}

	FfInitVals (ಧ್ರುವ SigMapView *sigmap, RTLIL::Module *module)
	{
		set(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_END

#endif
