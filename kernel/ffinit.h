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
	अचर SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	शून्य set(अचर SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		लागि (स्वतः wire : module->wires())
			यदि (wire->attributes.गन्ती(ID::init))
				process_wire(wire);
	}

	शून्य process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		लागि (पूर्णाङ्क i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			यदि (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				जारी;

			यदि (initbits.गन्ती(bit)) {
				यदि (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				जारी;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	शून्य set_parallel(अचर SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		अचर RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->wires_size(), 1000));
		ShardedVector<RTLIL::Wire*> init_wires(subpool);
		subpool.run([const_module, &init_wires](अचर ParallelDispatchThreadPool::RunCtx &ctx) {
			लागि (पूर्णाङ्क i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				यदि (wire->attributes.गन्ती(ID::init))
					init_wires.घुसाउ(ctx, wire);
			}
		});

		लागि (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State चालक()(RTLIL::SigBit bit) अचर
	{
		स्वतः it = initbits.find((*sigmap)(bit));
		यदि (it != initbits.end())
			फर्काऊ it->second.first;
		अन्यथा
			फर्काऊ State::Sx;
	}

	RTLIL::Const चालक()(अचर RTLIL::SigSpec &sig) अचर
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		लागि (स्वतः bit : sig)
			res_bits.थप_back((*this)(bit));
		फर्काऊ res_bits.build();
	}

	शून्य set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		स्वतः it = initbits.find(mbit);
		यदि (it != initbits.end())
			abit = it->second.second;
		अन्यथा यदि (val == State::Sx)
			फर्काऊ;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		स्वतः it2 = abit.wire->attributes.find(ID::init);
		यदि (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			यदि (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} अन्यथा यदि (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	शून्य set_init(अचर RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		लागि (पूर्णाङ्क i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	शून्य remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	शून्य remove_init(अचर RTLIL::SigSpec &sig)
	{
		लागि (स्वतः bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	शून्य move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		लागि (स्वतः cell : module->cells()) {
			यदि (cell->type != ID($barrier))
				जारी;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			लागि (पूर्णाङ्क i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		यदि (next.empty() || initbits.empty())
			फर्काऊ;
		pool<RTLIL::SigBit> inputs = आगत_nets(module), moved;
		लागि (स्वतः [from, to] : next) {
			RTLIL::State val = (*this)(from);
			यदि (val == State::Sx)
				जारी;
			moved.घुसाउ(from);
			लागि (पूर्णाङ्क i = 0; i < GetSize(next) && next.गन्ती((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			यदि (to_net.wire == nullptr || inputs.गन्ती(to_net))
				जारी;
			RTLIL::State to_val = (*this)(to);
			यदि (to_val == State::Sx)
				set_init(to, val);
			अन्यथा यदि (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		यदि (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> आगत_nets(RTLIL::Module *module) अचर
	{
		pool<RTLIL::SigBit> nets;
		लागि (स्वतः wire : module->wires())
			यदि (wire->port_आगत)
				लागि (स्वतः bit : (*sigmap)(wire))
					nets.घुसाउ(bit);
		फर्काऊ nets;
	}

	शून्य remove_net_inits(RTLIL::Module *module, अचर pool<RTLIL::SigBit> &nets)
	{
		लागि (स्वतः bit : nets)
			initbits.erase(bit);
		लागि (स्वतः wire : module->wires()) {
			स्वतः it = wire->attributes.find(ID::init);
			यदि (it == wire->attributes.end())
				जारी;
			लागि (पूर्णाङ्क i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				यदि (nets.गन्ती((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			यदि (it->second.is_fully_undef())
				wire->attributes.erase(it);
		}
	}

	शून्य clear()
	{
		initbits.clear();
	}

	FfInitVals (अचर SigMapView *sigmap, RTLIL::Module *module)
	{
		set(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_END

#endif
