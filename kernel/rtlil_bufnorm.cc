/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
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

#ضمن "kernel/yosys.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/modtools.h"

#ضمن <string.h>
#ضمن <algorithm>
#ضمن <optional>

YOSYS_NAMESPACE_BEGIN


ساكن فراغ buf_norm_تخم_queues(RTLIL::Module *module)
{
	// When entering buf normalized mode, we need the first module-level bufNormalize
	// call to know about all drivers, about all module ports (whether represented by
	// a cell or not) and about all used but undriven wires (whether represented by a
	// cell or not). We ensure this by enqueing all cell output ports and all wires.

	لكل (خپلکاری cell : module->cells())
	لكل (خپلکاری &conn : cell->connections()) {
		إذا (Get_اندازه(conn.دوهم) == 0 || (cell->پورټ_dir(conn.لومړی) != RTLIL::PD_OUTPUT && cell->پورټ_dir(conn.لومړی) != RTLIL::PD_INOUT))
			استمر;
		module->buf_norm_cell_کتار.ورننباسه(cell);
		module->buf_norm_cell_پورټ_کتار.emplace(cell, conn.لومړی);
	}
	لكل (خپلکاری wire : module->wires())
		module->buf_norm_wire_کتار.ورننباسه(wire);
}

فراغ RTLIL::Design::bufNormalize(منطقي enable)
{
	إذا (!enable)
	{
		إذا (!flagBufferedNormalized)
			أرجع;

		لكل (خپلکاری module : modules()) {
			module->buf_norm_cell_کتار.clear();
			module->buf_norm_wire_کتار.clear();
			module->buf_norm_cell_پورټ_کتار.clear();
			لكل (خپلکاری wire : module->wires()) {
				wire->driverCell_ = تش_اشاره;
				wire->driver_پورټ_2 = IdString();
			}
			module->buf_norm_connect_شاخص.clear();
		}

		flagBufferedNormalized = خطأ;
		أرجع;
	}

	إذا (!flagBufferedNormalized)
	{
		لكل (خپلکاری module : modules())
			buf_norm_تخم_queues(module);

		flagBufferedNormalized = ریښتیا;
	}

	لكل (خپلکاری module : modules())
		module->bufNormalize();
}

بنية بټ_drive_معلومات_t {
	صحيح drivers = 0;
	صحيح inout = 0;
	صحيح users = 0;
};

تعريفنوع ModWalker::پورټ_بټ_2 پورټ_بټ_2;

فراغ RTLIL::Module::bufNormalize()
{
	// Since this is kernel code, we only log with yosys_xtrace set to not get
	// in the way when using `debug` to debug specific passes.q
#تعریف_کړه xlog(...) افعل { إذا (yosys_xtrace) لوګاریتم("#X [bufnorm] " __VA_ARGS__); } طالما (0)

	إذا (!design->flagBufferedNormalized)
		أرجع;

	إذا (!buf_norm_cell_کتار.تشه() || !buf_norm_wire_کتار.تشه() || !connections_.تشه())
	{
		// Ensure that every enqueued input port is represented by a cell
		لكل (خپلکاری wire : buf_norm_wire_کتار) {
			إذا (wire->پورټ_ننوت && !wire->پورټ_output && Get_اندازه(wire) != 0) {
				إذا (wire->driverCell_ != تش_اشاره && wire->driverCell_->type != ID($ننوت_پورټ)) {
					wire->driverCell_ = تش_اشاره;
					wire->driver_پورټ_2 = IdString::Null;
				}
				إذا (wire->driverCell_ == تش_اشاره) {
					Cell *ننوت_پورټ_cell = جمع_کړه_Cell(NEW_ID, ID($ننوت_پورټ));
					ننوت_پورټ_cell->سېټ_Param(ID::پلنوالی_2, Get_اندازه(wire));
					ننوت_پورټ_cell->سېټ_پورټ(ID::Y, wire); // this hits the fast path that doesn't mutate the queues
				}
			}
		}

		// Next we will temporarily undo buf normalization locally for
		// everything enqueued. This means we will turn $buf and $connect back
		// into connections. When doing this we also need to enqueue the other
		// end of $buf and $connect cells, so we use a queue and do this until
		// reaching a fixed point.

		// While doing this, we will also discover all drivers fully connected
		// to enqueued wires. We keep track of which wires are driven by a
		// unique and full cell ports (in which case the wire can stay
		// connected to the port) and which cell ports will need to be
		// reconnected to a fresh intermediate wire to re-normalize the module.

		idict<Wire *> wire_کتار_entries; // Ordered queue of wires to process
		صحيح wire_کتار_pos = 0; // Index up to which we processed the wires

		// Wires with their unique driving cell port. We pick the first driver
		// we encounter, with the exception that we ensure that a module input
		// port can only get $input_port drivers and that $input_port drivers
		// cannot drive any other modules. If we reject an $input_port driver
		// because it's not driving an input port or because there already is
		// another $input_port driver for the same port, we also delete that
		// $input_port cell.
		dict<Wire *, std::pair<Cell *, IdString>> مستقیم_driven_wires;

		// Set of cell ports that need a fresh intermediate wire. These are all
		// cell ports that drive non-full-wire sigspecs, cell ports driving
		// module input ports, and cell ports driving wires that are already
		// driven.
		pool<std::pair<Cell *, IdString>> pending_ports;

		// This helper will be called for every output/inout cell port that is
		// already enqueued or becomes reachable when denormalizing $buf or
		// $connect cells.
		خپلکاری کتار_ته_ورزیاتول_cell_پورټ = [&](Cell *cell, IdString پورټ) {
			xlog("processing cell port %s.%s\n", cell, design->twines().unescaped_str(پورټ));

			// An empty cell type means the cell got removed
			إذا (cell->type.تشه())
				أرجع;


			SigSpec ثابت &sig = cell->get_پورټ(پورټ);

			// Make sure all wires of the cell port are enqueued, ensuring we
			// detect other connected drivers (output and inout).
			لكل (خپلکاری chunk : sig.chunks())
				إذا (chunk.is_wire())
					wire_کتار_entries(chunk.wire);

			إذا (cell->type == ID($buf) && cell->ځانګړتیاوې.تشه() && !cell->name.is_عامه()) {
				// For a plain `$buf` cell, we enqueue all wires on its input
				// side, bypass it using module level connections (skipping 'z
				// bits) and then remove the cell. Eventually the module level
				// connections will turn back into `$buf` and `$connect` cells,
				// but since we also need to handle externally added module
				// level connections, turning everything into connections first
				// simplifies the logic for doing so.

				// TODO: We could defer removing the $buf cells here, and
				// re-use them in case we would create a new identical cell
				// later.
				log_assert(پورټ == ID::Y);
				SigSpec sig_a = cell->get_پورټ(ID::A);
				SigSpec sig_y = sig;

				لكل (خپلکاری ثابت &s : {sig_a, sig})
					لكل (خپلکاری ثابت &chunk : s.chunks())
						إذا (chunk.wire)
							wire_کتار_entries(chunk.wire);

				إذا (sig_a.has_const(State::Sz)) {
					SigSpec new_a;
					SigSpec new_y;
					لكل (صحيح i = 0; i < Get_اندازه(sig_a); ++i) {
						Sig_بټ b = sig_a[i];
						إذا (b == State::Sz)
							استمر;
						new_a.append(b);
						new_y.append(sig_y[i]);
					}
					sig_a = std::move(new_a);
					sig_y = std::move(new_y);
				}

				إذا (!sig_y.تشه())
					connect(sig_y, sig_a);
				remove(cell);
				log_assert(Get_اندازه(buf_norm_wire_کتار) <= 1);
				buf_norm_wire_کتار.clear();
				أرجع;
			} وإلا إذا (cell->type == ID($ننوت_پورټ)) {
				log_assert(پورټ == ID::Y);
				إذا (sig.is_wire()) {
					Wire *w = sig.as_wire();
					إذا (w->پورټ_ننوت && !w->پورټ_output) {
						// An $input_port cell can only drive a full wire module input port
						خپلکاری [وموندل, inserted] = مستقیم_driven_wires.emplace(w, {cell, پورټ});
						إذا (!inserted || (وموندل->دوهم.لومړی == cell && وموندل->دوهم.دوهم == پورټ))
							أرجع;
					}
				}

				// If an `$input_port` cell isn't driving a full
				// input port wire, we remove it since the wires are still the
				// canonical source of module ports

				buf_norm_cell_کتار.ورننباسه(cell);
				remove(cell);
				log_assert(Get_اندازه(buf_norm_wire_کتار) <= 1);
				buf_norm_wire_کتار.clear();
				أرجع;
			}

			إذا (sig.is_wire()) {
				Wire *w = sig.as_wire();
				إذا (!w->پورټ_ننوت || w->پورټ_output) {
					// If the full cell port is connected to a full non-input-port wire, pick it as driver
					خپلکاری [وموندل, inserted] = مستقیم_driven_wires.emplace(w, {cell, پورټ});
					إذا (inserted || (وموندل->دوهم.لومړی == cell && وموندل->دوهم.دوهم == پورټ))
						أرجع;
				}
			}

			// Adds this port to the ports that need a fresh intermediate wire.
			// For full wires uniquely driven by a full output port, this isn't
			// reached due to the `return` above.
			pending_ports.emplace(cell, پورټ);
		};

		// We enqueue all enqueued wires for `$buf`/`$connect` processing (clearing the module level queue).
		لكل (خپلکاری wire : buf_norm_wire_کتار)
			wire_کتار_entries(wire);
		buf_norm_wire_کتار.clear();

		// Only after clearing the `buf_norm_wire_queue` are we allowed to call
		// enqueue_cell_port, since we're using assertions to check against
		// unintended wires being enqueued into `buf_norm_wire_queue` that
		// would prevent us from restoring the bufnorm invariants in a single
		// pass.

		// We process all explicitly enqueued cell ports (clearing the module level queue).
		لكل (خپلکاری ثابت &[cell, پورټ_name] : buf_norm_cell_پورټ_کتار)
			کتار_ته_ورزیاتول_cell_پورټ(cell, پورټ_name);
		buf_norm_cell_پورټ_کتار.clear();

		// We also enqueue all wires that saw newly added module level connections.
		لكل (خپلکاری &[a, b] : connections_)
			لكل (خپلکاری &sig : {a, b})
				لكل (خپلکاری ثابت &chunk : sig.chunks())
					إذا (chunk.wire)
						wire_کتار_entries(chunk.wire);

		// We then process all wires by processing known driving cell ports
		// (previously buf normalized) and following all `$connect` cells (that
		// have a dedicated module level index while the design is in buf
		// normalized mode).
		طالما (wire_کتار_pos < Get_اندازه(wire_کتار_entries)) {
			خپلکاری wire = wire_کتار_entries[wire_کتار_pos++];
			xlog("processing wire %s\n", wire);

			إذا (wire->driverCell_) {
				Cell *cell = wire->driverCell_;
				IdString پورټ = wire->driver_پورټ_2;
				کتار_ته_ورزیاتول_cell_پورټ(cell, پورټ);
			}

			طالما (ریښتیا) {
				خپلکاری وموندل = buf_norm_connect_شاخص.پیدا_کړه(wire);
				إذا (وموندل == buf_norm_connect_شاخص.پای_ټکی())
					اكسر;
				طالما (!وموندل->دوهم.تشه()) {
					Cell *connect_cell = *وموندل->دوهم.begin();
					log_assert(connect_cell->type == ID($connect));
					SigSpec ثابت &sig_a = connect_cell->get_پورټ(ID::A);
					SigSpec ثابت &sig_b = connect_cell->get_پورټ(ID::B);
					xlog("found $connect cell %s: %s <-> %s\n", connect_cell, log_signal(sig_a), log_signal(sig_b));
					لكل (خپلکاری &side : {sig_a, sig_b})
						لكل (خپلکاری chunk : side.chunks())
							إذا (chunk.wire)
								wire_کتار_entries(chunk.wire);
					connect(sig_a, sig_b);

					buf_norm_cell_کتار.ورننباسه(connect_cell);
					remove(connect_cell);
					log_assert(Get_اندازه(buf_norm_wire_کتار) <= 2);
					buf_norm_wire_کتار.clear();
				}
			}
		}

		// At this point we know all cell ports and wires that need to be
		// re-normalized and know their connectivity is represented by module
		// level connections.

		// As a first step for re-normalization we add all require intermediate
		// wires for cell output and inout ports.
		لكل (خپلکاری &[cell, پورټ] : pending_ports) {
			log_assert(cell->type != ID($ننوت_پورټ));
			log_assert(!cell->type.تشه());
			log_assert(!pending_deleted_cells.شمېر(cell));
			SigSpec ثابت &sig = cell->get_پورټ(پورټ);
			Wire *w = جمع_کړه_Wire(NEW_ID, Get_اندازه(sig));

			// We update the module level connections, `direct_driven_wires`
			// and `direct_driven_wires_conflicts` in such a way that they
			// correspond to what you would get if the intermediate wires had
			// been in place from the beginning.
			connect(sig, w);
			مستقیم_driven_wires.emplace(w, {cell, پورټ});
			cell->سېټ_پورټ(پورټ, w); // Hits the fast path that doesn't enqueue w
			wire_کتار_entries(w); // Needed so we pick up the sig <-> w connection
		}

		// At this point we're done with creating wires and know which ones are
		// fully driven by full output ports of existing cells.

		// First we clear the bufnorm data for all processed wires, all of
		// these will be reassigned later, but we use `driverCell_ == nullptr`
		// to keep track of the wires that we still have to update.
		لكل (خپلکاری wire : wire_کتار_entries) {
			wire->driverCell_ = تش_اشاره;
			wire->driver_پورټ_2 = IdString::Null;
		}

		// For the unique driving cell ports fully connected to a full wire, we
		// can update the bufnorm data right away. For all other wires we will
		// have to create new `$buf` cells.
		لكل (خپلکاری ثابت &[wire, cellport] : مستقیم_driven_wires) {
			wire->driverCell_ = cellport.لومړی;
			wire->driver_پورټ_2 = cellport.دوهم;
		}


		// To create fresh `$buf` cells for all remaining wires, we need to
		// process the module level connectivity to figure out what the input
		// of those `$buf` cells should be and to figure out whether we need
		// any `$connect` cells to represent bidirectional inout connections
		// (or driver conflicts).

		إذا (yosys_xtrace)
			لكل (خپلکاری ثابت &[lhs, rhs] : connections_)
				xlog("connection %s <-> %s\n", log_signal(lhs), log_signal(rhs));


		// We transfer the connectivity into a sigmap and then clear the module
		// level connections. This forgets about the structure of module level
		// connections, but bufnorm only guarantees that the connectivity as
		// maintained by a `SigMap` is preserved.
		SigMap sigmap(هذا);
		new_connections({});

		// We pick SigMap representatives by prioritizing input ports over
		// driven wires over other/unknown wires.
		لكل (منطقي ننوت_ports : {خطأ, ریښتیا}) {
			لكل (خپلکاری ثابت &[wire, cellport] : مستقیم_driven_wires) {
				إذا ((wire->پورټ_ننوت && !wire->پورټ_output) == ننوت_ports) {
					لكل (صحيح i = 0; i != Get_اندازه(wire); ++i) {
						Sig_بټ driver = Sig_بټ(wire, i);
						sigmap.database.promote(driver);
					}
				}
			}
		}

		// All three pool<SigBit> below are in terms of sigmapped bits
		// Bits that are known to have a unique driver that is an unconditional driver or one or more inout drivers
		pool<Sig_بټ> driven;
		// Bits that have multiple unconditional drivers, this forces the use of `$connect`
		pool<Sig_بټ> conflicted;
		// Bits that are driven by an inout driver
		pool<Sig_بټ> weakly_driven;

		لكل (خپلکاری ثابت &[wire, cellport] : مستقیم_driven_wires) {
			خپلکاری ثابت &[cell, پورټ] = cellport;
			لكل (صحيح i = 0; i != Get_اندازه(wire); ++i) {
				Sig_بټ driver = sigmap(Sig_بټ(wire, i));
				إذا (cell->type == ID($tribuf) || cell->پورټ_dir(پورټ) == RTLIL::PD_INOUT) {
					// We add inout drivers to `driven` in a separate loop below
					weakly_driven.ورننباسه(driver);
				} وإلا {
					// We remove driver conflicts from `driven` in a separate loop below
					منطقي inserted = driven.ورننباسه(driver).دوهم;
					إذا (!inserted)
						conflicted.ورننباسه(driver);
				}
			}
		}

		لكل (خپلکاری driver : weakly_driven)
			driven.ورننباسه(driver);

		// This only leaves the drivers matching `driven`'s definition above
		لكل (خپلکاری driver : conflicted)
			driven.erase(driver);

		// Having picked representatives and checked whether they are unique
		// drivers, we can turn the connecitivty of our sigmap back into $buf
		// and $connect cells.

		// Module level bitwise connections not representable by `$buf` cells
		pool<pair<Sig_بټ, Sig_بټ>> undirected_connections;

		// Starts out empty but is updated with the connectivity realized by freshly added `$buf` cells
		SigMap buf_connected;

		// For every enqueued wire, we compute a SigSpec of representative
		// drivers. If there are any bits without a unique driver we represent
		// that with `Sz`. If there are multiple drivers for a net, they become
		// connected via `$connect` cells but every wire of the net has the
		// corresponding bit still driven by a buffered `Sz`.
		لكل (خپلکاری wire : wire_کتار_entries) {
			// A zero-width wire carries no bits and needs no driver
			إذا (Get_اندازه(wire) == 0)
				استمر;

			SigSpec wire_drivers;
			لكل (صحيح i = 0; i < Get_اندازه(wire); ++i) {
				Sig_بټ بټ(wire, i);
				Sig_بټ mapped = sigmap(بټ);
				xlog("bit %s -> mapped %s\n", log_signal(بټ), log_signal(mapped));


				buf_connected.apply(بټ);
				buf_connected.جمع_کړه(بټ, mapped);
				buf_connected.database.promote(mapped);

				إذا (wire->driverCell_ == تش_اشاره) {
					إذا (!mapped.is_wire() || driven.شمېر(mapped)) {
						wire_drivers.append(mapped);
						استمر;
					} وإلا {
						wire_drivers.append(State::Sz);
					}
				}

				إذا (بټ < mapped)
					undirected_connections.emplace(بټ, mapped);
				وإلا إذا (mapped < بټ)
					undirected_connections.emplace(mapped, بټ);
			}

			إذا (wire->driverCell_ == تش_اشاره) {
				xlog("wire %s drivers %s\n", wire, log_signal(wire_drivers));
				جمع_کړه_Buf(NEW_ID, wire_drivers, wire);
			}
		}

		// Finally we group the bitwise connections to emit word-level $connect cells

		ساكن خپلکاری ترتیبول_کلی = [](std::pair<Sig_بټ, Sig_بټ> ثابت &p) {
			صحيح لومړی_دننه_واټن = p.لومړی.is_wire() ? p.لومړی.دننه_واټن : 0;
			صحيح دوهم_دننه_واټن = p.دوهم.is_wire() ? p.دوهم.دننه_واټن : 0;
			أرجع std::make_tuple(p.لومړی.wire, p.دوهم.wire, لومړی_دننه_واټن - دوهم_دننه_واټن, p);
		};

		undirected_connections.ترتیبول([](std::pair<Sig_بټ, Sig_بټ> ثابت &p, std::pair<Sig_بټ, Sig_بټ> ثابت &q) {
			أرجع ترتیبول_کلی(p) < ترتیبول_کلی(q);
		});

		SigSpec tmp_a, tmp_b;

		لكل (خپلکاری &[بټ_a, بټ_b] : undirected_connections) {
			tmp_a.append(بټ_a);
			tmp_b.append(بټ_b);
		}

		xlog("LHS: %s\n", log_signal(tmp_a));
		xlog("RHS: %s\n", log_signal(tmp_b));


		SigSpec sig_a, sig_b;
		Sig_بټ بل_a, بل_b;

		خپلکاری emit_connect_cell = [&]() {
			إذا (sig_a.تشه())
				أرجع;
			xlog("connect %s <-> %s\n", log_signal(sig_a), log_signal(sig_b));
			Cell *connect_cell = جمع_کړه_Cell(NEW_ID, ID($connect));
			connect_cell->سېټ_Param(ID::پلنوالی_2, Get_اندازه(sig_a));
			connect_cell->سېټ_پورټ(ID::A, sig_a);
			connect_cell->سېټ_پورټ(ID::B, sig_b);
			sig_a = SigSpec();
			sig_b = SigSpec();
		};

		لكل (خپلکاری &[بټ_a, بټ_b] : undirected_connections) {
			إذا (بټ_a == بټ_b)
				استمر;
			إذا (بټ_a != بل_a || بټ_b != بل_b)
				emit_connect_cell();

			sig_a.append(بټ_a);
			sig_b.append(بټ_b);
			بل_a = بټ_a;
			بل_b = بټ_b;
			إذا (بل_a.is_wire())
				بل_a.دننه_واټن++;
			إذا (بل_b.is_wire())
				بل_b.دننه_واټن++;

		}
		emit_connect_cell();

		buf_norm_cell_کتار.clear();

		log_assert(buf_norm_cell_پورټ_کتار.تشه());
		log_assert(buf_norm_wire_کتار.تشه());
		log_assert(connections_.تشه());
	}

	لكل (خپلکاری cell : pending_deleted_cells) {
		ړنګول cell;
	}
	pending_deleted_cells.clear();
}

فراغ RTLIL::Cell::unset_پورټ(RTLIL::IdString portname)
{
	RTLIL::SigSpec إشارة;
	خپلکاری conn_it = connections_.پیدا_کړه(portname);

	إذا (conn_it != connections_.پای_ټکی())
	{
		لكل (خپلکاری mon : module->monitors)
			mon->notify_connect(هذا, conn_it->لومړی, conn_it->دوهم, إشارة);

		إذا (module->design)
			لكل (خپلکاری mon : module->design->monitors)
				mon->notify_connect(هذا, conn_it->لومړی, conn_it->دوهم, إشارة);

		إذا (yosys_xtrace) {
			لوګاریتم("#X# Unconnect %s.%s.%s\n", هذا->module, هذا, PooledName(module, portname).unescape());
			log_backtrace("-X- ", yosys_xtrace-1);
		}

		إذا (module->design && module->design->flagBufferedNormalized) {
			إذا (conn_it->دوهم.is_wire()) {
				Wire *w = conn_it->دوهم.as_wire();
				إذا (w->driverCell_ == هذا && w->driver_پورټ_2 == portname) {
					w->driverCell_ = تش_اشاره;
					w->driver_پورټ_2 = IdString();
					module->buf_norm_wire_کتار.ورننباسه(w);
				} وإلا إذا (w->driverCell_) {
					log_assert(w->driverCell_->get_پورټ(w->driver_پورټ_2) == w);
				}
			}

			إذا (type == ID($connect)) {
				لكل (خپلکاری &[پورټ, sig] : connections_) {
					لكل (خپلکاری &chunk : sig.chunks()) {
						إذا (!chunk.wire)
							استمر;
						خپلکاری it = module->buf_norm_connect_شاخص.پیدا_کړه(chunk.wire);
						إذا (it == module->buf_norm_connect_شاخص.پای_ټکی())
							استمر;
						it->دوهم.erase(هذا);
						إذا (it->دوهم.تشه())
							module->buf_norm_connect_شاخص.erase(it);
					}
				}
				connections_.erase(conn_it);
				لكل (خپلکاری &[پورټ, sig] : connections_) {
					لكل (خپلکاری &chunk : sig.chunks()) {
						إذا (!chunk.wire)
							استمر;
						module->buf_norm_connect_شاخص[chunk.wire].ورننباسه(هذا);
					}
				}
				أرجع;
			}
		}

		connections_.erase(conn_it);
	}
}

فراغ RTLIL::Cell::سېټ_پورټ(RTLIL::IdString portname, RTLIL::SigSpec إشارة)
{
	خپلکاری r = connections_.ورننباسه(portname);
	خپلکاری conn_it = r.لومړی;
	إذا (!r.دوهم && conn_it->دوهم == إشارة)
		أرجع;

	لكل (خپلکاری mon : module->monitors)
		mon->notify_connect(هذا, conn_it->لومړی, conn_it->دوهم, إشارة);

	إذا (module->design)
		لكل (خپلکاری mon : module->design->monitors)
			mon->notify_connect(هذا, conn_it->لومړی, conn_it->دوهم, إشارة);

	إذا (yosys_xtrace) {
		لوګاریتم("#X# Connect %s.%s.%s = %s (%d)\n", هذا->module, هذا, PooledName(module, portname).unescape(), log_signal(إشارة), Get_اندازه(إشارة));
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	إذا (module->design && module->design->flagBufferedNormalized)
	{
		// We eagerly clear a driver that got disconnected by changing this port connection
		إذا (conn_it->دوهم.is_wire()) {
			Wire *w = conn_it->دوهم.as_wire();
			إذا (w->driverCell_ == هذا && w->driver_پورټ_2 == portname) {
				w->driverCell_ = تش_اشاره;
				w->driver_پورټ_2 = IdString();
				module->buf_norm_wire_کتار.ورننباسه(w);
			}
		}

		خپلکاری dir = پورټ_dir(portname);
		// This is a fast path that handles connecting a full driverless wire to an output port,
		// everything else is goes through the bufnorm queues and is handled during the next
		// bufNormalize call
		إذا ((dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) && إشارة.is_wire()) {
			Wire *w = إشارة.as_wire();
			إذا (w->driverCell_ == تش_اشاره && (
						(w->پورټ_ننوت && !w->پورټ_output) == (type == ID($ننوت_پورټ)))) {
				w->driverCell_ = هذا;
				w->driver_پورټ_2 = portname;

				conn_it->دوهم = std::move(إشارة);
				أرجع;
			}
		}

		إذا (dir == RTLIL::PD_OUTPUT || dir == RTLIL::PD_INOUT) {
			module->buf_norm_cell_کتار.ورننباسه(هذا);
			module->buf_norm_cell_پورټ_کتار.emplace(هذا, portname);
		} وإلا {
			لكل (خپلکاری &chunk : إشارة.chunks())
				إذا (chunk.wire != تش_اشاره && chunk.wire->driverCell_ == تش_اشاره)
					module->buf_norm_wire_کتار.ورننباسه(chunk.wire);
		}

		إذا (type == ID($connect)) {
			لكل (خپلکاری &[پورټ, sig] : connections_) {
				لكل (خپلکاری &chunk : sig.chunks()) {
					إذا (!chunk.wire)
						استمر;
					خپلکاری it = module->buf_norm_connect_شاخص.پیدا_کړه(chunk.wire);
					إذا (it == module->buf_norm_connect_شاخص.پای_ټکی())
						استمر;
					it->دوهم.erase(هذا);
					إذا (it->دوهم.تشه())
						module->buf_norm_connect_شاخص.erase(it);
				}
			}
			conn_it->دوهم = std::move(إشارة);
			لكل (خپلکاری &[پورټ, sig] : connections_) {
				لكل (خپلکاری &chunk : sig.chunks()) {
					إذا (!chunk.wire)
						استمر;
					module->buf_norm_connect_شاخص[chunk.wire].ورننباسه(هذا);
				}
			}
			أرجع;
		}
	}
	conn_it->دوهم = std::move(إشارة);

}

فراغ RTLIL::Design::جمع_کړه(RTLIL::Module *module)
{
	log_assert(modules_.شمېر(module->name) == 0);
	log_assert(refcount_modules_ == 0);
	modules_[module->name] = module;
	module->design = هذا;

	لكل (خپلکاری mon : monitors)
		mon->notify_module_جمع_کړه(module);

	إذا (yosys_xtrace) {
		لوګاریتم("#X# New Module: %s\n", module);
		log_backtrace("-X- ", yosys_xtrace-1);
	}

	إذا (flagBufferedNormalized) {
		buf_norm_تخم_queues(module);
		module->bufNormalize();
	}
}

YOSYS_NAMESPACE_پای_ټکی
