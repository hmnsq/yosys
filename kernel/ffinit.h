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

#অন্তর্ভাৱ "kernel/yosys.h"
#অন্তর্ভাৱ "kernel/sigtools.h"
#অন্তর্ভাৱ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

সংরচনা FfInitVals
{
	ধ্রুৱ SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	শূন্য set(ধ্রুৱ SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		বাবে (auto wire : module->wires())
			যদি (wire->attributes.গুনতি(ID::init))
				process_wire(wire);
	}

	শূন্য process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		বাবে (পূর্ণাঙ্ক i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			যদি (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				অনুৱর্ত;

			যদি (initbits.গুনতি(bit)) {
				যদি (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				অনুৱর্ত;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	শূন্য set_parallel(ধ্রুৱ SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ধ্রুৱ RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->wires_size(), 1000));
		ShardedVector<RTLIL::Wire*> init_wires(subpool);
		subpool.run([const_module, &init_wires](ধ্রুৱ ParallelDispatchThreadPool::RunCtx &ctx) {
			বাবে (পূর্ণাঙ্ক i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				যদি (wire->attributes.গুনতি(ID::init))
					init_wires.ঢোকাও(ctx, wire);
			}
		});

		বাবে (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State operator()(RTLIL::SigBit bit) ধ্রুৱ
	{
		auto it = initbits.find((*sigmap)(bit));
		যদি (it != initbits.end())
			ঘূৰাই it->second.first;
		অন্যথা
			ঘূৰাই State::Sx;
	}

	RTLIL::Const operator()(ধ্রুৱ RTLIL::SigSpec &sig) ধ্রুৱ
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		বাবে (auto bit : sig)
			res_bits.ঠেলো_back((*this)(bit));
		ঘূৰাই res_bits.build();
	}

	শূন্য set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		auto it = initbits.find(mbit);
		যদি (it != initbits.end())
			abit = it->second.second;
		অন্যথা যদি (val == State::Sx)
			ঘূৰাই;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		auto it2 = abit.wire->attributes.find(ID::init);
		যদি (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			যদি (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} অন্যথা যদি (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	শূন্য set_init(ধ্রুৱ RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		বাবে (পূর্ণাঙ্ক i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	শূন্য remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	শূন্য remove_init(ধ্রুৱ RTLIL::SigSpec &sig)
	{
		বাবে (auto bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	শূন্য move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		বাবে (auto cell : module->cells()) {
			যদি (cell->type != ID($barrier))
				অনুৱর্ত;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			বাবে (পূর্ণাঙ্ক i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		যদি (next.empty() || initbits.empty())
			ঘূৰাই;
		pool<RTLIL::SigBit> inputs = প্ৰবেশ_nets(module), moved;
		বাবে (auto [from, to] : next) {
			RTLIL::State val = (*this)(from);
			যদি (val == State::Sx)
				অনুৱর্ত;
			moved.ঢোকাও(from);
			বাবে (পূর্ণাঙ্ক i = 0; i < GetSize(next) && next.গুনতি((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			যদি (to_net.wire == nullptr || inputs.গুনতি(to_net))
				অনুৱর্ত;
			RTLIL::State to_val = (*this)(to);
			যদি (to_val == State::Sx)
				set_init(to, val);
			অন্যথা যদি (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		যদি (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> প্ৰবেশ_nets(RTLIL::Module *module) ধ্রুৱ
	{
		pool<RTLIL::SigBit> nets;
		বাবে (auto wire : module->wires())
			যদি (wire->port_প্ৰবেশ)
				বাবে (auto bit : (*sigmap)(wire))
					nets.ঢোকাও(bit);
		ঘূৰাই nets;
	}

	শূন্য remove_net_inits(RTLIL::Module *module, ধ্রুৱ pool<RTLIL::SigBit> &nets)
	{
		বাবে (auto bit : nets)
			initbits.erase(bit);
		বাবে (auto wire : module->wires()) {
			auto it = wire->attributes.find(ID::init);
			যদি (it == wire->attributes.end())
				অনুৱর্ত;
			বাবে (পূর্ণাঙ্ক i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				যদি (nets.গুনতি((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			যদি (it->second.is_fully_undef())
				wire->attributes.erase(it);
		}
	}

	শূন্য clear()
	{
		initbits.clear();
	}

	FfInitVals (ধ্রুৱ SigMapView *sigmap, RTLIL::Module *module)
	{
		set(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_END

#endif
