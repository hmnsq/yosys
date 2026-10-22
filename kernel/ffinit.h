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

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/sigtools.h"
#अन्तर्भाव "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

संरचना FfInitVals
{
	ध्रुव SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	शून्य set(ध्रुव SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		प्रति (auto wire : module->wires())
			जइ (wire->attributes.गणनम्(ID::init))
				process_wire(wire);
	}

	शून्य process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		प्रति (पूर्णाङ्क i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			जइ (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				अनुवर्त;

			जइ (initbits.गणनम्(bit)) {
				जइ (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				अनुवर्त;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	शून्य set_parallel(ध्रुव SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		ध्रुव RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->wires_size(), 1000));
		ShardedVector<RTLIL::Wire*> init_wires(subpool);
		subpool.run([const_module, &init_wires](ध्रुव ParallelDispatchThreadPool::RunCtx &ctx) {
			प्रति (पूर्णाङ्क i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				जइ (wire->attributes.गणनम्(ID::init))
					init_wires.निवेशय(ctx, wire);
			}
		});

		प्रति (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State operator()(RTLIL::SigBit bit) ध्रुव
	{
		auto it = initbits.find((*sigmap)(bit));
		जइ (it != initbits.end())
			प्रत्यागम it->second.first;
		अण्णहा
			प्रत्यागम State::Sx;
	}

	RTLIL::Const operator()(ध्रुव RTLIL::SigSpec &sig) ध्रुव
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		प्रति (auto bit : sig)
			res_bits.प्रक्षिप_back((*this)(bit));
		प्रत्यागम res_bits.build();
	}

	शून्य set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		auto it = initbits.find(mbit);
		जइ (it != initbits.end())
			abit = it->second.second;
		अण्णहा जइ (val == State::Sx)
			प्रत्यागम;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		auto it2 = abit.wire->attributes.find(ID::init);
		जइ (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			जइ (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} अण्णहा जइ (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	शून्य set_init(ध्रुव RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		प्रति (पूर्णाङ्क i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	शून्य remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	शून्य remove_init(ध्रुव RTLIL::SigSpec &sig)
	{
		प्रति (auto bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	शून्य move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		प्रति (auto cell : module->cells()) {
			जइ (cell->type != ID($barrier))
				अनुवर्त;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			प्रति (पूर्णाङ्क i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		जइ (next.empty() || initbits.empty())
			प्रत्यागम;
		pool<RTLIL::SigBit> inputs = निवेशः_nets(module), moved;
		प्रति (auto [from, to] : next) {
			RTLIL::State val = (*this)(from);
			जइ (val == State::Sx)
				अनुवर्त;
			moved.निवेशय(from);
			प्रति (पूर्णाङ्क i = 0; i < GetSize(next) && next.गणनम्((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			जइ (to_net.wire == nullptr || inputs.गणनम्(to_net))
				अनुवर्त;
			RTLIL::State to_val = (*this)(to);
			जइ (to_val == State::Sx)
				set_init(to, val);
			अण्णहा जइ (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		जइ (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> निवेशः_nets(RTLIL::Module *module) ध्रुव
	{
		pool<RTLIL::SigBit> nets;
		प्रति (auto wire : module->wires())
			जइ (wire->port_निवेशः)
				प्रति (auto bit : (*sigmap)(wire))
					nets.निवेशय(bit);
		प्रत्यागम nets;
	}

	शून्य remove_net_inits(RTLIL::Module *module, ध्रुव pool<RTLIL::SigBit> &nets)
	{
		प्रति (auto bit : nets)
			initbits.erase(bit);
		प्रति (auto wire : module->wires()) {
			auto it = wire->attributes.find(ID::init);
			जइ (it == wire->attributes.end())
				अनुवर्त;
			प्रति (पूर्णाङ्क i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				जइ (nets.गणनम्((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			जइ (it->second.is_fully_undef())
				wire->attributes.erase(it);
		}
	}

	शून्य clear()
	{
		initbits.clear();
	}

	FfInitVals (ध्रुव SigMapView *sigmap, RTLIL::Module *module)
	{
		set(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_END

#endif
