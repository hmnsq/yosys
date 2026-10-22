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

#నిర్వచితం_కాకపోతే FFINIT_H
#నిర్వచించు FFINIT_H

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/sigtools.h"
#అన్తర్భావ "kernel/threading.h"

YOSYS_NAMESPACE_BEGIN

నిర్మాణం FfInitVals
{
	స్థిరరాశి SigMapView *sigmap;
	dict<Sig_బిట్టు, std::pair<State,Sig_బిట్టు>> initbits;

	శూన్య సమితి(స్థిరరాశి SigMapView *sigmap_, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();
		కొరకు (స్వయంచాలక wire : module->wires())
			ఒకవేళ (wire->లక్షణాలు.లెక్క(ID::init))
				process_wire(wire);
	}

	శూన్య process_wire(RTLIL::Wire *wire)
	{
		SigSpec wirebits = (*sigmap)(wire);
		Const initval = wire->లక్షణాలు.at(ID::init);

		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wirebits) && i < Get_కొలత(initval); i++)
		{
			Sig_బిట్టు బిట్టు = wirebits[i];
			State val = initval[i];

			ఒకవేళ (val != State::S0 && val != State::S1 && బిట్టు.wire != nullptr)
				కొనసాగు;

			ఒకవేళ (initbits.లెక్క(బిట్టు)) {
				ఒకవేళ (initbits.at(బిట్టు).మొదటి != val)
					log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
							log_signal(బిట్టు), log_signal(Sig_బిట్టు(wire, i)),
							log_signal(val), log_signal(initbits.at(బిట్టు).మొదటి));
				కొనసాగు;
			}

			initbits[బిట్టు] = std::make_pair(val,Sig_బిట్టు(wire,i));
		}
	}

	శూన్య సమితి_parallel(స్థిరరాశి SigMapView *sigmap_, ParallelDispatchThreadPool &thread_pool, RTLIL::Module *module)
	{
		sigmap = sigmap_;
		initbits.clear();

		స్థిరరాశి RTLIL::Module *const_module = module;
		ParallelDispatchThreadPool::Subpool subpool(thread_pool, ThreadPool::work_pool_కొలత(0, module->wires_కొలత(), 1000));
		Sharded_సదిశ<RTLIL::Wire*> init_wires(subpool);
		subpool.నిరంతరం([const_module, &init_wires](స్థిరరాశి ParallelDispatchThreadPool::నిరంతరం_Ctx &ctx) {
			కొరకు (పూర్ణాంకం i : ctx.అంశం_range(const_module->wires_కొలత())) {
				RTLIL::Wire *wire = const_module->wire_at(i);
				ఒకవేళ (wire->లక్షణాలు.లెక్క(ID::init))
					init_wires.చొప్పించు(ctx, wire);
			}
		});

		కొరకు (RTLIL::Wire *wire : init_wires)
			process_wire(wire);
	}

	RTLIL::State క్రియాకారకం()(RTLIL::Sig_బిట్టు బిట్టు) స్థిరరాశి
	{
		స్వయంచాలక it = initbits.కనుగొను((*sigmap)(బిట్టు));
		ఒకవేళ (it != initbits.చివర())
			తిరిగి it->రెండవ.మొదటి;
		లేకపోతే
			తిరిగి State::Sx;
	}

	RTLIL::Const క్రియాకారకం()(స్థిరరాశి RTLIL::SigSpec &sig) స్థిరరాశి
	{
		RTLIL::Const::Builder res_బిట్లు(Get_కొలత(sig));
		కొరకు (స్వయంచాలక బిట్టు : sig)
			res_బిట్లు.నెట్టు_వెనుక((*this)(బిట్టు));
		తిరిగి res_బిట్లు.build();
	}

	శూన్య సమితి_init(RTLIL::Sig_బిట్టు బిట్టు, RTLIL::State val)
	{
		Sig_బిట్టు mbit = (*sigmap)(బిట్టు);
		Sig_బిట్టు abit = బిట్టు;
		స్వయంచాలక it = initbits.కనుగొను(mbit);
		ఒకవేళ (it != initbits.చివర())
			abit = it->రెండవ.రెండవ;
		లేకపోతే ఒకవేళ (val == State::Sx)
			తిరిగి;
		log_assert(abit.wire);
		initbits[mbit] = std::make_pair(val,abit);
		స్వయంచాలక it2 = abit.wire->లక్షణాలు.కనుగొను(ID::init);
		ఒకవేళ (it2 != abit.wire->లక్షణాలు.చివర()) {
			it2->రెండవ.సమితి(abit.స్థానభ్రంశం, val);
			ఒకవేళ (it2->రెండవ.is_fully_undef())
				abit.wire->లక్షణాలు.erase(it2);
		} లేకపోతే ఒకవేళ (val != State::Sx) {
			Const cval(State::Sx, Get_కొలత(abit.wire));
			cval.సమితి(abit.స్థానభ్రంశం, val);
			abit.wire->లక్షణాలు[ID::init] = cval;
		}
	}

	శూన్య సమితి_init(స్థిరరాశి RTLIL::SigSpec &sig, RTLIL::Const val)
	{
		log_assert(Get_కొలత(sig) == Get_కొలత(val));
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(sig); i++)
			సమితి_init(sig[i], val[i]);
	}

	శూన్య remove_init(RTLIL::Sig_బిట్టు బిట్టు)
	{
		సమితి_init(బిట్టు, State::Sx);
	}

	శూన్య remove_init(స్థిరరాశి RTLIL::SigSpec &sig)
	{
		కొరకు (స్వయంచాలక బిట్టు : sig)
			remove_init(బిట్టు);
	}

	// Move $barrier output init value to it's input
	శూన్య move_barrier_inits(RTLIL::Module *module)
	{
		dict<RTLIL::Sig_బిట్టు, RTLIL::Sig_బిట్టు> తదుపరి;
		కొరకు (స్వయంచాలక cell : module->cells()) {
			ఒకవేళ (cell->type != ID($barrier))
				కొనసాగు;
			RTLIL::SigSpec sig_y = (*sigmap)(cell->get_పోర్టు(ID::Y)), sig_a = cell->get_పోర్టు(ID::A);
			కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(sig_y); i++)
				తదుపరి[sig_y[i]] = sig_a[i];
		}
		ఒకవేళ (తదుపరి.ఖాళీలు() || initbits.ఖాళీలు())
			తిరిగి;
		pool<RTLIL::Sig_బిట్టు> inputs = ఆదానం_nets(module), moved;
		కొరకు (స్వయంచాలక [from, గమ్యం] : తదుపరి) {
			RTLIL::State val = (*this)(from);
			ఒకవేళ (val == State::Sx)
				కొనసాగు;
			moved.చొప్పించు(from);
			కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(తదుపరి) && తదుపరి.లెక్క((*sigmap)(గమ్యం)); i++)
				గమ్యం = తదుపరి.at((*sigmap)(గమ్యం));
			RTLIL::Sig_బిట్టు గమ్యం_net = (*sigmap)(గమ్యం);
			ఒకవేళ (గమ్యం_net.wire == nullptr || inputs.లెక్క(గమ్యం_net))
				కొనసాగు;
			RTLIL::State గమ్యం_val = (*this)(గమ్యం);
			ఒకవేళ (గమ్యం_val == State::Sx)
				సమితి_init(గమ్యం, val);
			లేకపోతే ఒకవేళ (గమ్యం_val != val)
				log_error("Conflicting init values for signal %s (%s = %s != %s).\n",
						log_signal(గమ్యం), log_signal(from), log_signal(val), log_signal(గమ్యం_val));
		}
		ఒకవేళ (!moved.ఖాళీలు())
			remove_net_inits(module, moved);
	}

	pool<RTLIL::Sig_బిట్టు> ఆదానం_nets(RTLIL::Module *module) స్థిరరాశి
	{
		pool<RTLIL::Sig_బిట్టు> nets;
		కొరకు (స్వయంచాలక wire : module->wires())
			ఒకవేళ (wire->పోర్టు_ఆదానం)
				కొరకు (స్వయంచాలక బిట్టు : (*sigmap)(wire))
					nets.చొప్పించు(బిట్టు);
		తిరిగి nets;
	}

	శూన్య remove_net_inits(RTLIL::Module *module, స్థిరరాశి pool<RTLIL::Sig_బిట్టు> &nets)
	{
		కొరకు (స్వయంచాలక బిట్టు : nets)
			initbits.erase(బిట్టు);
		కొరకు (స్వయంచాలక wire : module->wires()) {
			స్వయంచాలక it = wire->లక్షణాలు.కనుగొను(ID::init);
			ఒకవేళ (it == wire->లక్షణాలు.చివర())
				కొనసాగు;
			కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(wire) && i < Get_కొలత(it->రెండవ); i++)
				ఒకవేళ (nets.లెక్క((*sigmap)(RTLIL::Sig_బిట్టు(wire, i))))
					it->రెండవ.సమితి(i, State::Sx);
			ఒకవేళ (it->రెండవ.is_fully_undef())
				wire->లక్షణాలు.erase(it);
		}
	}

	శూన్య clear()
	{
		initbits.clear();
	}

	FfInitVals (స్థిరరాశి SigMapView *sigmap, RTLIL::Module *module)
	{
		సమితి(sigmap, module);
	}

	FfInitVals () {}
};


YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
