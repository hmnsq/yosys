#ضمن "kernel/twine.h"
#ضمن "kernel/hashcons_impl.h"
#ضمن "kernel/log.h"

YOSYS_NAMESPACE_BEGIN

template بنية HashConsPool<TwinePool, TwineNode, IdString>;

std::vector<TwineNode> StaticTwines::nodes_;

فراغ StaticTwines::init() {
	אם (ready())
		החזר;
	log_assert(nodes_.empty());
	nodes_.reserve(STATIC_TWINE_END);
	עבור (ثابت حرف *name : ID::static_names)
		nodes_.emplace_back(std::string_view(name));
}

ثابت TwineNode &StaticTwines::node(size_t idx) { החזר nodes_[idx]; }
منطقي StaticTwines::ready() { החזר nodes_.size() == STATIC_TWINE_END; }

int64_t twine_gc_ns;
صحيح twine_gc_count;

std::string IdString::handle_token() ثابت {
	החזר stringf("%s@%zu", isPublic() ? "$pub" : "$priv", untag().raw());
}

size_t IdString::handle_token_prefix(std::string_view token, منطقي &is_public) {
	אם (token.substr(0, 5) == "$pub@") {
		is_public = אמת;
		החזר 5;
	}
	אם (token.substr(0, 6) == "$priv@") {
		is_public = שקר;
		החזר 6;
	}
	החזר 0;
}

std::string ID::str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	std::string result = ref.isPublic() ? "\\" : "";
	result += static_names[idx.raw()];
	החזר result;
}

std::string ID::unescaped_str(IdString ref) {
	IdString idx = ref.untag();
	log_assert(idx.raw() < STATIC_TWINE_END);
	החזר static_names[idx.raw()];
}

TwineSpec::TwineSpec(Leaf v) : data(std::move(v)) {}
TwineSpec::TwineSpec(Suffix v) : data(std::move(v)) {}
TwineSpec::TwineSpec(AutoSuffix v) : data(std::move(v)) {}

فراغ SmallString::store(std::string_view content)
{
	log_assert(content.size() <= MAX_LEN);
	release();
	len_ = content.size();
	אם (len_ > INLINE_CAP) {
		ptr_ = جديد حرف[len_];
		memcpy(ptr_, content.data(), len_);
	} אחרת {
		memcpy(inl_, content.data(), len_);
	}
}

فراغ SmallString::release()
{
	אם (len_ > INLINE_CAP)
		delete[] ptr_;
	len_ = 0;
}

SmallString::SmallString(std::string_view content) { store(content); }

SmallString::SmallString(ثابت SmallString &other) { store(other.view()); }

SmallString::SmallString(SmallString &&other) noexcept : len_(other.len_)
{
	memcpy(inl_, other.inl_, INLINE_CAP);
	other.len_ = 0;
}

SmallString &SmallString::operator=(ثابت SmallString &other)
{
	אם (هذا != &other)
		store(other.view());
	החזר *هذا;
}

SmallString &SmallString::operator=(SmallString &&other) noexcept
{
	אם (هذا == &other)
		החזר *هذا;
	release();
	memcpy(inl_, other.inl_, INLINE_CAP);
	len_ = other.len_;
	other.len_ = 0;
	החזר *هذا;
}

SmallString::~SmallString() { release(); }

TwineNode::TwineNode(TwineNode &&other) noexcept
	: text_(std::move(other.text_)), prefix_(other.prefix_), hash_(other.hash_)
{
	other.prefix_ = DEAD;
}

TwineNode &TwineNode::operator=(TwineNode &&other) noexcept
{
	אם (هذا == &other)
		החזר *هذا;
	text_ = std::move(other.text_);
	prefix_ = other.prefix_;
	hash_ = other.hash_;
	other.prefix_ = DEAD;
	החזר *هذا;
}

std::string TwineSpec::content_str() ثابت {
	אם (auto *leaf = std::get_if<Leaf>(&data))
		החזר leaf->s;
	auto &autosfx = std::get<AutoSuffix>(data);
	החזר *autosfx.prefix + autosfx.tail;
}

ساكن std::pair<std::string_view, منطقي> unescape_name(std::string_view name) {
	אם (!name.empty() && name[0] == '\\')
		החזר {name.substr(1), אמת};
	החזר {name, שקר};
}

ساكن فراغ check_name(std::string_view name) {
	log_assert(name[0] == '$' || name[0] == '\\');
	עבור (حرف ch : name)
		אם ((unsigned)ch <= (unsigned)' ')
			log_error("Found control character or space (0x%02x) in string '%s' which is not allowed in RTLIL identifiers\n", ch, std::string(name));
}

TwinePool::TwinePool() : serial_(next_serial()) {}
TwinePool::TwinePool(ثابت TwinePool& other)
	: HashConsPool(other), auto_prefixes(other.auto_prefixes), serial_(next_serial()) {}
TwinePool::TwinePool(TwinePool&& other)
	: HashConsPool(std::move(other)), auto_prefixes(std::move(other.auto_prefixes)), serial_(next_serial()) {}

TwinePool& TwinePool::operator=(ثابت TwinePool& other) {
	אם (هذا == &other)
		החזר *هذا;
	HashConsPool::operator=(other);
	auto_prefixes = other.auto_prefixes;
	serial_ = next_serial();
	החזר *هذا;
}

TwinePool& TwinePool::operator=(TwinePool&& other) {
	אם (هذا == &other)
		החזר *هذا;
	HashConsPool::operator=(std::move(other));
	auto_prefixes = std::move(other.auto_prefixes);
	serial_ = next_serial();
	החזר *هذا;
}

منطقي TwinePool::owns(IdString ref) ثابت {
	החזר ref.serial() == 0 || ref.serial() == serial_;
}

IdString TwinePool::stamp(IdString ref) ثابت {
	אם (ref == IdString::Null || ID::is_static(ref))
		החזר ref;
	החזר ref.stamped(serial_);
}

فراغ TwinePool::check_owned(IdString ref) ثابت {
	אם constexpr (IdString::MAX_SERIAL != 0)
		log_assert(owns(ref));
}

ثابت TwineNode& TwinePool::operator[](IdString ref) ثابت {
	check_owned(ref);
	החזר HashConsPool::operator[](ref);
}

size_t TwinePool::gc(pool<IdString>& roots) {
	עבור (auto &it : auto_prefixes)
		roots.insert(it.second);
	החזר HashConsPool::gc(roots);
}

ثابت TwineNode& TwinePool::static_node(size_t idx) { החזר StaticTwines::node(idx); }
فراغ TwinePool::check_ready() { log_assert(StaticTwines::ready()); }

size_t TwinePool::next_serial() {
	ساكن size_t counter = 0;
	size_t serial = ++counter;
	אם constexpr (IdString::MAX_SERIAL == 0)
		החזר serial;
	אחרת
		החזר serial > IdString::MAX_SERIAL ? (serial % IdString::MAX_SERIAL) + 1 : serial;
}

IdString TwinePool::add_inner(TwineNode t) {
	אם (free_list.empty() && STATIC_COUNT + backing.size() > IdString::MAX_INDEX)
		log_error("Out of twine handles: a design may name at most %zu distinct twines.\n",
				IdString::MAX_INDEX - STATIC_COUNT);
	החזר HashConsPool::add_inner(std::move(t));
}

فراغ TwinePool::dump(IdString ref, std::ostream& os) ثابت {
	ثابت TwineNode& twine = (*هذا)[ref];
	بدل (twine.kind()) {
	حالة TwineNode::Kind::Dead:
		os << "Dead()";
		اكسر;
	حالة TwineNode::Kind::Leaf:
		os << "Leaf(\"" << twine.text() << "\")";
		اكسر;
	حالة TwineNode::Kind::Suffix:
		os << "Suffix(prefix: ";
		dump(twine.prefix(), os);
		os << ", tail: \"" << twine.text() << "\")";
		اكسر;
	}
	אם (ref.isPublic())
		os << " pub";
}

فراغ TwinePool::append_str(IdString ref, std::string& out) ثابت {
	אם (ref == IdString::Null)
		החזר;
	אם (ref.isPublic())
		out += '\\';
	ثابت TwineNode& twine = (*هذا)[ref];
	بدل (twine.kind()) {
	حالة TwineNode::Kind::Dead:
		اكسر;
	حالة TwineNode::Kind::Suffix:
		append_str(twine.prefix(), out);
		[[fallthrough]];
	حالة TwineNode::Kind::Leaf:
		out += twine.text();
		اكسر;
	}
}

std::string TwinePool::str(IdString ref) ثابت {
	std::string out;
	append_str(ref, out);
	החזר out;
}

std::string TwinePool::unescaped_str(IdString ref) ثابت {
	החזר str(ref.untag());
}

uint32_t TwinePool::content_hash(uint32_t prefix, std::string_view text) ثابت {
	uint32_t hash = prefix == TwineNode::NO_PREFIX ? 0 : (*هذا)[IdString(prefix)].hash();
	החזר TwineNode::extend_hash(hash, text);
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text) ثابت {
	החזר find_content(prefix, text, content_hash(prefix, text));
}

IdString TwinePool::intern(uint32_t prefix, std::string_view text) {
	log_assert(!Multithreading::active());
	uint32_t hash = content_hash(prefix, text);
	IdString ref = find_content(prefix, text, hash);
	אם (ref != IdString::Null)
		החזר ref;
	החזר add_inner(TwineNode{prefix, text, hash});
}

IdString TwinePool::find(std::string_view name) ثابت {
	אם (name.empty())
		החזר IdString::Null;
	check_name(name);
	auto [content, is_public] = unescape_name(name);
	החזר stamp(find_content(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::find(TwineSpec t) ثابت {
	אם (std::holds_alternative<TwineSpec::AutoSuffix>(t.data))
		החזר stamp(find_content(TwineNode::NO_PREFIX, t.content_str()));
	אם (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		החזר stamp(find_content(TwineNode::NO_PREFIX, leaf->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	החזר stamp(find_content((uint32_t)sfx.prefix.untag().raw(), sfx.tail).tag(sfx.prefix.isPublic()));
}

IdString TwinePool::add(TwineSpec t) {
	אם (auto *ap = std::get_if<TwineSpec::AutoSuffix>(&t.data))
		החזר add(auto_prefix(ap->prefix), ap->tail);
	אם (auto *leaf = std::get_if<TwineSpec::Leaf>(&t.data))
		החזר stamp(intern(TwineNode::NO_PREFIX, leaf->s));
	ثابت TwineSpec::Suffix &sfx = std::get<TwineSpec::Suffix>(t.data);
	החזר add(sfx.prefix, sfx.tail);
}

IdString TwinePool::auto_prefix(ثابت std::string *prefix) {
	auto it = auto_prefixes.find(prefix);
	אם (it != auto_prefixes.end())
		החזר it->second;
	IdString ref = intern(TwineNode::NO_PREFIX, *prefix);
	auto_prefixes[prefix] = ref;
	החזר ref;
}

IdString TwinePool::add(IdString prefix, std::string_view tail) {
	החזר stamp(intern((uint32_t)prefix.untag().raw(), tail).tag(prefix.isPublic()));
}

IdString TwinePool::add(std::string_view s) {
	אם (s.empty())
		החזר IdString::Null;
	check_name(s);
	auto [content, is_public] = unescape_name(s);
	החזר stamp(intern(TwineNode::NO_PREFIX, content).tag(is_public));
}

IdString TwinePool::copy_from(ثابت TwinePool& src, IdString ref) {
	IdString found = find_from(src, ref);
	אם (found != IdString::Null || ref == IdString::Null)
		החזר found;
	ثابت TwineNode& t = src[ref.untag()];
	אם (t.is_dead())
		החזר IdString::Null;
	uint32_t prefix = TwineNode::NO_PREFIX;
	אם (t.is_suffix())
		prefix = (uint32_t)copy_from(src, t.prefix()).untag().raw();
	החזר stamp(intern(prefix, t.text()).tag(ref.isPublic()));
}

std::string TwinePool::ref_token(IdString ref) ثابت {
	החזר "#" + std::to_string((uint64_t)serial_) + ":"
		+ std::to_string((uint64_t)stamp(ref).bits());
}

IdString TwinePool::ref_from_token(std::string_view token) ثابت {
	אם (token.size() < 2 || token[0] != '#')
		החזר IdString::Null;
	size_t sep = token.find(':');
	אם (sep == std::string_view::npos)
		החזר IdString::Null;
	size_t fields[2] = {0, 0};
	std::string_view parts[2] = {token.substr(1, sep - 1), token.substr(sep + 1)};
	עבור (صحيح i = 0; i < 2; i++) {
		אם (parts[i].empty())
			החזר IdString::Null;
		עבור (حرف c : parts[i]) {
			אם (c < '0' || c > '9')
				החזר IdString::Null;
			fields[i] = fields[i] * 10 + (c - '0');
		}
	}
	אם (fields[0] != serial_)
		החזר IdString::Null;
	IdString ref(fields[1]);
	אם (ref == IdString::Null)
		החזר IdString::Null;
	החזר is_live(ref) ? ref : IdString::Null;
}

/**
 * TwineSegments holds a sequence of string_views refering to the strings
 * an IdString is composed of. It's used for lightweight comparisons.
 * The sequences is implemented in a "small vector" style,
 * so that IdStrings with few segments fit into the `inline_segs` field,
 * and anything beyond that is in `spill`.
 */
بنية TwineSegments {
	ساكن constexpr size_t SEGMENT_INLINE_DEPTH = 8;
	TwineSegments(ثابت TwinePool &pool, IdString ref);
	TwineSegments(ثابت TwinePool &pool, IdString head, std::string_view tail);
	std::string_view peek();
	فراغ advance(size_t n);
	size_t total_size() ثابت;
خاص:
	فراغ push(std::string_view seg);
	std::string_view *segs();
	ثابت std::string_view *segs() ثابت;
	std::string_view inline_segs[SEGMENT_INLINE_DEPTH];
	std::vector<std::string_view> spill;
	size_t count = 0;
	size_t pos = 0;
	size_t off = 0;
};

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString ref)
{
	אם (ref == IdString::Null)
		החזר;
	עבור (IdString cur = ref.untag(); ;) {
		ثابت TwineNode &t = pool[cur];
		بدل (t.kind()) {
		حالة TwineNode::Kind::Suffix:
			push(t.text());
			cur = t.prefix();
			استمر;
		حالة TwineNode::Kind::Leaf:
			push(t.text());
			اكسر;
		حالة TwineNode::Kind::Dead:
			اكسر;
		}
		اكسر;
	}
	אם (ref.isPublic())
		push("\\");
	std::reverse(segs(), segs() + count);
}

TwineSegments::TwineSegments(ثابت TwinePool &pool, IdString head, std::string_view tail)
	: TwineSegments(pool, head)
{
	push(tail);
}

std::string_view TwineSegments::peek()
{
	כלעוד (pos < count) {
		std::string_view seg = segs()[pos];
		אם (off < seg.size())
			החזר seg.substr(off);
		pos++;
		off = 0;
	}
	החזר {};
}

فراغ TwineSegments::advance(size_t n) { off += n; }

size_t TwineSegments::total_size() ثابت
{
	size_t total = 0;
	עבור (size_t i = 0; i < count; i++)
		total += segs()[i].size();
	החזר total;
}

فراغ TwineSegments::push(std::string_view seg)
{
	אם (spill.empty() && count < SEGMENT_INLINE_DEPTH) {
		inline_segs[count++] = seg;
		החזר;
	}
	אם (spill.empty())
		spill.assign(inline_segs, inline_segs + count);
	spill.push_back(seg);
	count++;
}

std::string_view *TwineSegments::segs() { החזר spill.empty() ? inline_segs : spill.data(); }
ثابت std::string_view *TwineSegments::segs() ثابت { החזר spill.empty() ? inline_segs : spill.data(); }

size_t TwinePool::str_size(IdString ref) ثابت
{
	החזר TwineSegments(*هذا, ref).total_size();
}

منطقي TwinePool::begins_with(IdString ref, std::string_view prefix) ثابت
{
	TwineSegments segs(*هذا, ref);
	כלעוד (!prefix.empty()) {
		std::string_view seg = segs.peek();
		אם (seg.empty())
			החזר שקר;
		size_t n = std::min(seg.size(), prefix.size());
		אם (std::memcmp(seg.data(), prefix.data(), n) != 0)
			החזר שקר;
		segs.advance(n);
		prefix.remove_prefix(n);
	}
	החזר אמת;
}

منطقي TwinePool::name_equal(IdString ref, std::string_view name) ثابت
{
	החזר str_size(ref) == name.size() && begins_with(ref, name);
}

ساكن صحيح compare_segments(TwineSegments &sa, TwineSegments &sb)
{
	כלעוד (אמת) {
		std::string_view x = sa.peek(), y = sb.peek();
		אם (x.empty() || y.empty())
			החזר x.empty() ? (y.empty() ? 0 : -1) : 1;
		size_t n = std::min(x.size(), y.size());
		אם (صحيح diff = std::memcmp(x.data(), y.data(), n); diff != 0)
			החזר diff;
		sa.advance(n);
		sb.advance(n);
	}
}

IdString TwinePool::find_content(uint32_t prefix, std::string_view text, uint32_t hash) ثابت {
	החזר find_hashed(hash, [&](IdString ref) {
		ثابت TwineNode &t = (*هذا)[ref];
		אם (t == TwineNode::Key{prefix, text})
			החזר אמת;
		IdString head = prefix == TwineNode::NO_PREFIX ? IdString::Null : IdString(prefix);
		TwineSegments sa(*هذا, ref), sb(*هذا, head, text);
		החזר compare_segments(sa, sb) == 0;
	});
}

IdString TwinePool::find_from(ثابت TwinePool& src, IdString ref) ثابت {
	אם (ref == IdString::Null || ID::is_static(ref))
		החזר ref;
	IdString idx = ref.untag();
	ثابت TwineNode& t = src[idx];
	אם (t.is_dead())
		החזר IdString::Null;
	IdString found = find_hashed(t.hash(), [&](IdString cand) {
		TwineSegments sa(*هذا, cand), sb(src, idx);
		החזר compare_segments(sa, sb) == 0;
	});
	החזר stamp(found.tag(ref.isPublic()));
}

IdString TwinePool::prefix_of(IdString ref) ثابت {
	ثابت TwineNode &t = (*هذا)[ref];
	אם (!t.is_suffix())
		החזר IdString::Null;
	החזר stamp(t.prefix().tag(ref.isPublic()));
}

صحيح TwinePool::compare_by_name(IdString a, IdString b) ثابت
{
	אם (a == b)
		החזר 0;
	אם (a == IdString::Null)
		החזר -1;
	אם (b == IdString::Null)
		החזר 1;

	אם (a.isPublic() == b.isPublic()) {
		ثابت TwineNode &ta = (*هذا)[a.untag()];
		ثابت TwineNode &tb = (*هذا)[b.untag()];
		אם (ta.is_leaf() && tb.is_leaf())
			החזר ta.text().compare(tb.text());
		אם (ta.is_suffix() && tb.is_suffix() && ta.prefix() == tb.prefix())
			החזר ta.text().compare(tb.text());
	}

	TwineSegments sa(*هذا, a), sb(*هذا, b);
	החזר compare_segments(sa, sb);
}

YOSYS_NAMESPACE_END
