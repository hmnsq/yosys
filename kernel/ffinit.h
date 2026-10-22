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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/sigtools.h"
#અન્તર્ભાવ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

સંરચના FfInitVals
{
	અચલ SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	શૂન્ય set(અચલ SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		માટે (auto wire : module->wires())
			જો (wire->attributes.ગણતરી(ID::init))
				process_wire(wire);
	}

	શૂન્ય process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		માટે (પૂર્ણાંક i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			જો (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				ચાલુ;

			જો (initbits.ગણતરી(bit)) {
				જો (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				ચાલુ;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	શૂન્ય set_parallel(અચલ SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		અચલ RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->wires_size(), 1000));
		ShardedVector<RTLIL::Wire*> init_wires(subpool);
		subpool.run([const_module, &init_wires](અચલ ParallelDispatchThreadPool::RunCtx &ctx) {
			માટે (પૂર્ણાંક i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				જો (wire->attributes.ગણતરી(ID::init))
					init_wires.ઉમેરો(ctx, wire);
			}
		});

		માટે (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State operator()(RTLIL::SigBit bit) અચલ
	{
		auto it = initbits.find((*sigmap)(bit));
		જો (it != initbits.end())
			પાછા it->second.first;
		નહીંતર
			પાછા State::Sx;
	}

	RTLIL::Const operator()(અચલ RTLIL::SigSpec &sig) અચલ
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		માટે (auto bit : sig)
			res_bits.ધકેલો_back((*this)(bit));
		પાછા res_bits.build();
	}

	શૂન્ય set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		auto it = initbits.find(mbit);
		જો (it != initbits.end())
			abit = it->second.second;
		નહીંતર જો (val == State::Sx)
			પાછા;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		auto it2 = abit.wire->attributes.find(ID::init);
		જો (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			જો (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} નહીંતર જો (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	શૂન્ય set_init(અચલ RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		માટે (પૂર્ણાંક i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	શૂન્ય remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	શૂન્ય remove_init(અચલ RTLIL::SigSpec &sig)
	{
		માટે (auto bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	શૂન્ય move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		માટે (auto cell : module->cells()) {
			જો (cell->type != ID($barrier))
				ચાલુ;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			માટે (પૂર્ણાંક i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		જો (next.empty() || initbits.empty())
			પાછા;
		pool<RTLIL::SigBit> inputs = આગત_nets(module), moved;
		માટે (auto [from, to] : next) {
			RTLIL::State val = (*this)(from);
			જો (val == State::Sx)
				ચાલુ;
			moved.ઉમેરો(from);
			માટે (પૂર્ણાંક i = 0; i < GetSize(next) && next.ગણતરી((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			જો (to_net.wire == nullptr || inputs.ગણતરી(to_net))
				ચાલુ;
			RTLIL::State to_val = (*this)(to);
			જો (to_val == State::Sx)
				set_init(to, val);
			નહીંતર જો (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		જો (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> આગત_nets(RTLIL::Module *module) અચલ
	{
		pool<RTLIL::SigBit> nets;
		માટે (auto wire : module->wires())
			જો (wire->port_આગત)
				માટે (auto bit : (*sigmap)(wire))
					nets.ઉમેરો(bit);
		પાછા nets;
	}

	શૂન્ય remove_net_inits(RTLIL::Module *module, અચલ pool<RTLIL::SigBit> &nets)
	{
		માટે (auto bit : nets)
			initbits.erase(bit);
		માટે (auto wire : module->wires()) {
			auto it = wire->attributes.find(ID::init);
			જો (it == wire->attributes.end())
				ચાલુ;
			માટે (પૂર્ણાંક i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				જો (nets.ગણતરી((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			જો (it->second.is_fully_undef())
				wire->attributes.erase(it);
		}
	}

	શૂન્ય clear()
	{
		initbits.clear();
	}

	FfInitVals (અચલ SigMapView *sigmap, RTLIL::Module *module)
	{
		set(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_END

#endif
