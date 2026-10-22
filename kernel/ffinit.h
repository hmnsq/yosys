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
	स्थिरांक SigMapView *sigmap;
	dict<SigBit, std::pair<State,SigBit>> initbits;

	शून्य set(स्थिरांक SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		साठी (स्वतः wire : module->wires())
			जर (wire->attributes.मोजणी(ID::init))
				process_wire(wire);
	}

	शून्य process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->attributes.at(ID::init);

		साठी (पूर्णांक i = 0; i < GetSize(wirebits) && i < GetSize(initval); i++)
		{
			SigBit bit = wirebits[i];
			State val = initval[i];

			जर (val != State::S0 && val != State::S1 && bit.wire != nullptr)
				सुरूठेवा;

			जर (initbits.मोजणी(bit)) {
				जर (initbits.at(bit).first != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(bit), log_signal(SigBit(wire, i)),
							log_signal(val), log_signal(initbits.at(bit).first));
				सुरूठेवा;
			}

			initbits[bit] = std::make_pair(val,SigBit(wire,i));
		}
	}

	शून्य set_parallel(स्थिरांक SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		स्थिरांक RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_size(0, module->wires_size(), 1000));
		ShardedVector<RTLIL::Wire*> init_wires(subpool);
		subpool.run([const_module, &init_wires](स्थिरांक ParallelDispatchThreadPool::RunCtx &ctx) {
			साठी (पूर्णांक i : ctx.item_range(const_module->wires_size())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				जर (wire->attributes.मोजणी(ID::init))
					init_wires.घाला(ctx, wire);
			}
		});

		साठी (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State चालक()(RTLIL::SigBit bit) स्थिरांक
	{
		स्वतः it = initbits.find((*sigmap)(bit));
		जर (it != initbits.end())
			परत it->second.first;
		अन्यथा
			परत State::Sx;
	}

	RTLIL::Const चालक()(स्थिरांक RTLIL::SigSpec &sig) स्थिरांक
	{
		RTLIL::Const::Builder res_bits(GetSize(sig));
		साठी (स्वतः bit : sig)
			res_bits.ढकला_back((*this)(bit));
		परत res_bits.build();
	}

	शून्य set_init(RTLIL::SigBit bit, RTLIL::State val)
	{
		SigBit mbit = (*sigmap)(bit);
		SigBit abit = bit;
		स्वतः it = initbits.find(mbit);
		जर (it != initbits.end())
			abit = it->second.second;
		अन्यथा जर (val == State::Sx)
			परत;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		स्वतः it2 = abit.wire->attributes.find(ID::init);
		जर (it2 != abit.wire->attributes.end()) {
			it2->second.set(abit.offset, val);
			जर (it2->second.is_fully_undef())
				abit.wire->attributes.erase(it2);
		} अन्यथा जर (val != State::Sx) {
			Const cval(State::Sx, GetSize(abit.wire));
			cval.set(abit.offset, val);
			abit.wire->attributes[ID::init] = cval;
		}
	}

	शून्य set_init(स्थिरांक RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(GetSize(sig) == GetSize(val));
		साठी (पूर्णांक i = 0; i < GetSize(sig); i++)
			set_init(sig[i], val[i]);
	}

	शून्य remove_init(RTLIL::SigBit bit)
	{
		set_init(bit, State::Sx);
	}

	शून्य remove_init(स्थिरांक RTLIL::SigSpec &sig)
	{
		साठी (स्वतः bit : sig)
			remove_init(bit);
	}

	// Move $barrier output init value to it's input
	शून्य move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::SigBit, RTLIL::SigBit> next;
		साठी (स्वतः cell : module->cells()) {
			जर (cell->type != ID($barrier))
				सुरूठेवा;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->getPort(ID::Y)), sig_a = cell->getPort(ID::A);
			साठी (पूर्णांक i = 0; i < GetSize(sig_y); i++)
				next[sig_y[i]] = sig_a[i];
		}
		जर (next.empty() || initbits.empty())
			परत;
		pool<RTLIL::SigBit> inputs = आदान_nets(module), moved;
		साठी (स्वतः [from, to] : next) {
			RTLIL::State val = (*this)(from);
			जर (val == State::Sx)
				सुरूठेवा;
			moved.घाला(from);
			साठी (पूर्णांक i = 0; i < GetSize(next) && next.मोजणी((*sigmap)(to)); i++)
				to = next.at((*sigmap)(to));
			RTLIL::SigBit to_net = (*sigmap)(to);
			जर (to_net.wire == nullptr || inputs.मोजणी(to_net))
				सुरूठेवा;
			RTLIL::State to_val = (*this)(to);
			जर (to_val == State::Sx)
				set_init(to, val);
			अन्यथा जर (to_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(to), log_signal(from), log_signal(val), log_signal(to_val));
		}
		जर (!moved.empty())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::SigBit> आदान_nets(RTLIL::Module *module) स्थिरांक
	{
		pool<RTLIL::SigBit> nets;
		साठी (स्वतः wire : module->wires())
			जर (wire->port_आदान)
				साठी (स्वतः bit : (*sigmap)(wire))
					nets.घाला(bit);
		परत nets;
	}

	शून्य remove_net_inits(RTLIL::Module *module, स्थिरांक pool<RTLIL::SigBit> &nets)
	{
		साठी (स्वतः bit : nets)
			initbits.erase(bit);
		साठी (स्वतः wire : module->wires()) {
			स्वतः it = wire->attributes.find(ID::init);
			जर (it == wire->attributes.end())
				सुरूठेवा;
			साठी (पूर्णांक i = 0; i < GetSize(wire) && i < GetSize(it->second); i++)
				जर (nets.मोजणी((*sigmap)(RTLIL::SigBit(wire, i))))
					it->second.set(i, State::Sx);
			जर (it->second.is_fully_undef())
				wire->attributes.erase(it);
		}
	}

	शून्य clear()
	{
		initbits.clear();
	}

	FfInitVals (स्थिरांक SigMapView *sigmap, RTLIL::Module *module)
	{
		set(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_END

#endif
